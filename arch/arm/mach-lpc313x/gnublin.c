/*  arch/arm/mach-lpc313x/gnublin.c
 *
 *  Copyright (C) 2012 Ingo Albrecht
 *
 *  GNUBLIN board init routines.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include <mach/board.h>
#include <mach/gpio.h>

#define GPIO_LED1    GPIO_GPIO3
#define GPIO_SPI_CS0 GPIO_GPIO11


static int mci_init(u32 slot_id)
{
	return 0;
}

static void mci_exit(u32 slot_id)
{
}

static int mci_get_ocr(u32 slot_id)
{
	return MMC_VDD_32_33 | MMC_VDD_33_34;
}

static void mci_setpower(u32 slot_id, u32 volt)
{
}

static struct resource lpc313x_mci_resources[] = {
	[0] = {
	       .start = IO_SDMMC_PHYS,
	       .end = IO_SDMMC_PHYS + IO_SDMMC_SIZE,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = IRQ_MCI,
	       .end = IRQ_MCI,
	       .flags = IORESOURCE_IRQ,
	       },
};

static struct lpc313x_mci_board lpc_h3131_mci_platform_data = {
	.num_slots = 1,
	.detect_delay_ms = 250,
	.init = mci_init,
	.exit = mci_exit,
	.get_ocr = mci_get_ocr,
	.setpower = mci_setpower,
	.slot = {
		[0] = {
			.bus_width = 4,
			.detect_pin = -1,
			.wp_pin = -1,
		},
	},
};

static u64 mci_dmamask = 0xffffffffUL;
static struct platform_device lpc313x_mci_device = {
	.name = "lpc313x_mmc",
	.num_resources = ARRAY_SIZE(lpc313x_mci_resources),
	.dev = {
		.dma_mask = &mci_dmamask,
		.coherent_dma_mask = 0xffffffff,
		.platform_data = &lpc_h3131_mci_platform_data,
		},
	.resource = lpc313x_mci_resources,
};


#if defined(CONFIG_SPI_LPC313X)
static struct resource lpc313x_spi_resources[] = {
	[0] = {
	       .start = SPI_PHYS,
	       .end = SPI_PHYS + SZ_4K - 1,
	       .flags = IORESOURCE_MEM,
	       },
	[1] = {
	       .start = IRQ_SPI,
	       .end = IRQ_SPI,
	       .flags = IORESOURCE_IRQ,
	       },
};

static void spi_set_cs_state(int cs_num, int state)
{
	int gpio;

	/* Select pin */
	switch(cs_num) {
	case 0:
		gpio = GPIO_SPI_CS0;
		break;
	}

	/* Set GPO state for CS */
	gpio_set_value(gpio, state);
}

struct lpc313x_spics_cfg lpc313x_stdspics_cfg[] = {
	/* SPI CS0 */
	[0] = {
	       .spi_spo = 0,	/* Low clock between transfers */
	       .spi_sph = 0,	/* Data capture on first clock edge (high edge with spi_spo=0) */
	       .spi_cs_set = spi_set_cs_state,
	},
};

struct lpc313x_spi_cfg lpc313x_spidata = {
	.num_cs = ARRAY_SIZE(lpc313x_stdspics_cfg),
	.spics_cfg = lpc313x_stdspics_cfg,
};

static u64 lpc313x_spi_dma_mask = 0xffffffffUL;
static struct platform_device lpc313x_spi_device = {
	.name = "spi_lpc313x",
	.id = 0,
	.dev = {
		.dma_mask = &lpc313x_spi_dma_mask,
		.coherent_dma_mask = 0xffffffffUL,
		.platform_data = &lpc313x_spidata,
		},
	.num_resources = ARRAY_SIZE(lpc313x_spi_resources),
	.resource = lpc313x_spi_resources,
};

#if defined(CONFIG_SPI_SPIDEV)
/* SPIDEV driver registration */
static int __init lpc313x_spidev_register(void)
{
	struct spi_board_info info = {
		.modalias = "spidev",
		.max_speed_hz = 1000000,
		.bus_num = 0,
		.chip_select = 0,
	};

	return spi_register_board_info(&info, 1);
}

arch_initcall(lpc313x_spidev_register);
#endif /* defined(CONFIG_SPI_SPIDEV */

#endif /* defined(CONFIG_SPI_LPC313X) */


#if defined(CONFIG_LEDS_GPIO_PLATFORM)
static struct gpio_led leds[] = {
    {
	.name   = "led1:red",
	.default_trigger = "heartbeat",
	.gpio   = GPIO_LED1,
    }
};

static struct gpio_led_platform_data leds_pdata = {
    .num_leds  = ARRAY_SIZE(leds),
    .leds      = leds,
};

static struct platform_device leds_device = {
    .name   = "leds-gpio",
    .id     = -1,
    .dev    = {
	.platform_data = &leds_pdata,
    },
};
#endif /* defined(CONFIG_LEDS_GPIO_PLATFORM) */


static struct i2c_board_info gnublin_i2c_devices[] __initdata = {
};


static struct platform_device *devices[] __initdata = {
	&lpc313x_mci_device,
#if defined(CONFIG_SPI_SPIDEV)
	&lpc313x_spi_device,
#endif
#if defined(CONFIG_LEDS_GPIO_PLATFORM)
	&leds_device
#endif
};

static struct map_desc io_desc[] __initdata = {
	{
	 .virtual = io_p2v(EXT_SRAM0_PHYS),
	 .pfn = __phys_to_pfn(EXT_SRAM0_PHYS),
	 .length = SZ_4K,
	 .type = MT_DEVICE},
	{
	 .virtual = io_p2v(EXT_SRAM1_PHYS + 0x10000),
	 .pfn = __phys_to_pfn(EXT_SRAM1_PHYS + 0x10000),
	 .length = SZ_4K,
	 .type = MT_DEVICE},
	{
	 .virtual = io_p2v(IO_SDMMC_PHYS),
	 .pfn = __phys_to_pfn(IO_SDMMC_PHYS),
	 .length = IO_SDMMC_SIZE,
	 .type = MT_DEVICE},
	{
	 .virtual = io_p2v(IO_USB_PHYS),
	 .pfn = __phys_to_pfn(IO_USB_PHYS),
	 .length = IO_USB_SIZE,
	 .type = MT_DEVICE},
};

static void __init gnublin_init(void)
{
	lpc313x_init();

	platform_add_devices(devices, ARRAY_SIZE(devices));

#if defined(CONFIG_SPI_LPC313X)
	gpio_request(GPIO_SPI_CS0, "spi.cs0");
	gpio_direction_output(GPIO_SPI_CS0, 1);
#endif

	lpc313x_register_i2c_devices();

	i2c_register_board_info(0, gnublin_i2c_devices,
				ARRAY_SIZE(gnublin_i2c_devices));
}

static void __init gnublin_map_io(void)
{
	lpc313x_map_io();

	iotable_init(io_desc, ARRAY_SIZE(io_desc));
}

void lpc313x_vbus_power(int enable)
{
}

EXPORT_SYMBOL(lpc313x_vbus_power);

#if defined(CONFIG_MACH_GNUBLIN)
MACHINE_START(GNUBLIN, "embedded projects GNUBLIN")
    /* Maintainer:  */
    .phys_io = IO_APB01_PHYS,
    .io_pg_offst = (io_p2v(IO_APB01_PHYS) >> 18) & 0xfffc,
    .boot_params = 0x30000100,
    .map_io = gnublin_map_io,
    .init_irq = lpc313x_init_irq,
    .timer = &lpc313x_timer,
    .init_machine = gnublin_init
MACHINE_END
#endif
