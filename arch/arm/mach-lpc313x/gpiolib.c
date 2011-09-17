/*
 * gpiolib.c - GPIOLIB platform support for NXP LPC313x
 *
 * Author: Ingo Albrecht <prom@berlin.ccc.de>
 *
 * Copyright 2010 (c) Ingo Albrecht
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/seq_file.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>

#include <mach/gpio.h>
#include <mach/irqs.h>
#include <mach/registers.h>

struct lpc313x_gpio_pin {
	char *name;
	int event_id;
};

struct lpc313x_gpio_chip {
	struct gpio_chip  chip;
	void __iomem     *regbase;
	struct lpc313x_gpio_pin *pins;
};

#define to_lpc313x_gpio_chip(c) container_of(c, struct lpc313x_gpio_chip, chip)

static int lpc313x_gpiolib_request(struct gpio_chip *chip, unsigned offset);
static void lpc313x_gpiolib_free(struct gpio_chip *chip, unsigned offset);
static int lpc313x_gpiolib_direction_input(struct gpio_chip *chip,
					   unsigned offset);
static int lpc313x_gpiolib_direction_output(struct gpio_chip *chip,
					    unsigned offset, int val);
static int lpc313x_gpiolib_get(struct gpio_chip *chip, unsigned offset);
static void lpc313x_gpiolib_set(struct gpio_chip *chip,
				unsigned offset, int val);
static int lpc313x_gpiolib_to_irq(struct gpio_chip *chip,
				unsigned offset);
static void lpc313x_gpiolib_dbg_show(struct seq_file *s,
				     struct gpio_chip *chip);

#define LPC313X_GPIO_PIN(pname, pevent) \
	{              						\
		.name = pname, 					\
		.event_id = pevent,				\
	}

#define LPC313X_GPIO_CHIP(name, basereg, base_gpio, nr_gpio, pinfo) \
    {                                                               \
    .chip = {                                                     	\
			.label            = name,                             	\
            .request          = lpc313x_gpiolib_request,          	\
            .free             = lpc313x_gpiolib_free,             	\
            .direction_input  = lpc313x_gpiolib_direction_input,  	\
            .direction_output = lpc313x_gpiolib_direction_output, 	\
            .get              = lpc313x_gpiolib_get,              	\
            .set              = lpc313x_gpiolib_set,              	\
            .to_irq           = lpc313x_gpiolib_to_irq,             \
            .dbg_show         = lpc313x_gpiolib_dbg_show,         	\
            .base             = (base_gpio),                      	\
            .ngpio            = (nr_gpio),                        	\
        },                                                        	\
    .regbase = ((void *)basereg),                                 	\
    .pins = ((struct lpc313x_gpio_pin *)pinfo), 					\
    }

static struct lpc313x_gpio_pin gpio_pins_gpio[] = {
		LPC313X_GPIO_PIN("GPIO1", EVT_GPIO1),
		LPC313X_GPIO_PIN("GPIO0", EVT_GPIO0),
		LPC313X_GPIO_PIN("GPIO2", EVT_GPIO2),
		LPC313X_GPIO_PIN("GPIO3", EVT_GPIO3),
		LPC313X_GPIO_PIN("GPIO4", EVT_GPIO4),
		LPC313X_GPIO_PIN("GPIO11", EVT_GPIO11),
		LPC313X_GPIO_PIN("GPIO12", EVT_GPIO12),
		LPC313X_GPIO_PIN("GPIO13", EVT_GPIO13),
		LPC313X_GPIO_PIN("GPIO14", EVT_GPIO14),
		LPC313X_GPIO_PIN("GPIO15", EVT_GPIO15),
		LPC313X_GPIO_PIN("GPIO16", EVT_GPIO16),
		LPC313X_GPIO_PIN("GPIO17", EVT_GPIO17),
		LPC313X_GPIO_PIN("GPIO18", EVT_GPIO18),
		LPC313X_GPIO_PIN("GPIO19", EVT_GPIO19),
		LPC313X_GPIO_PIN("GPIO20", EVT_GPIO20),
};

static struct lpc313x_gpio_pin gpio_pins_ebi_mci[] = {
		LPC313X_GPIO_PIN("mGPIO9",           EVT_mGPIO9),
		LPC313X_GPIO_PIN("mGPIO6",           EVT_mGPIO6),
		LPC313X_GPIO_PIN("mLCD_DB_7",        EVT_mLCD_DB_7),
		LPC313X_GPIO_PIN("mLCD_DB_4",        EVT_mLCD_DB_4),
		LPC313X_GPIO_PIN("mLCD_DB_2",        EVT_mLCD_DB_2),
		LPC313X_GPIO_PIN("mNAND_RYBN0",      EVT_mNAND_RYBN0),
		LPC313X_GPIO_PIN("mI2STX_CLK0",      EVT_mI2STX_CLK0),
		LPC313X_GPIO_PIN("mI2STX_BCK0",      EVT_mI2STX_BCK0),
		LPC313X_GPIO_PIN("EBI_A_1_CLE",      EVT_EBI_A_1_CLE),
		LPC313X_GPIO_PIN("EBI_NCAS_BLOUT_0", EVT_EBI_NCAS_BLOUT_0),
		LPC313X_GPIO_PIN("mLCD_DB_0",        EVT_mLCD_DB_0),
		LPC313X_GPIO_PIN("EBI_DQM_0_NOE",    EVT_EBI_DQM_0_NOE),
		LPC313X_GPIO_PIN("mLCD_CSB",         EVT_mLCD_CSB),
		LPC313X_GPIO_PIN("mLCD_DB_1",        EVT_mLCD_DB_1),
		LPC313X_GPIO_PIN("mLCD_E_RD",        EVT_mLCD_E_RD),
		LPC313X_GPIO_PIN("mLCD_RS",          EVT_mLCD_RS),
		LPC313X_GPIO_PIN("mLCD_RW_WR",       EVT_mLCD_RW_WR),
		LPC313X_GPIO_PIN("mLCD_DB_3",        EVT_mLCD_DB_3),
		LPC313X_GPIO_PIN("mLCD_DB_5",        EVT_mLCD_DB_5),
		LPC313X_GPIO_PIN("mLCD_DB_6",        EVT_mLCD_DB_6),
		LPC313X_GPIO_PIN("mLCD_DB_8",        EVT_mLCD_DB_8),
		LPC313X_GPIO_PIN("mLCD_DB_9",        EVT_mLCD_DB_9),
		LPC313X_GPIO_PIN("mLCD_DB_10",       EVT_mLCD_DB_10),
		LPC313X_GPIO_PIN("mLCD_DB_11",       EVT_mLCD_DB_11),
		LPC313X_GPIO_PIN("mLCD_DB_12",       EVT_mLCD_DB_12),
		LPC313X_GPIO_PIN("mLCD_DB_13",       EVT_mLCD_DB_13),
		LPC313X_GPIO_PIN("mLCD_DB_14",       EVT_mLCD_DB_14),
		LPC313X_GPIO_PIN("mLCD_DB_15",       EVT_mLCD_DB_15),
		LPC313X_GPIO_PIN("mGPIO5",           EVT_mGPIO5),
		LPC313X_GPIO_PIN("mGPIO7",           EVT_mGPIO7),
		LPC313X_GPIO_PIN("mGPIO8",           EVT_mGPIO8),
		LPC313X_GPIO_PIN("mGPIO10",          EVT_mGPIO10),
};

static struct lpc313x_gpio_pin gpio_pins_ebi_i2stx_0[] = {
		LPC313X_GPIO_PIN("mNAND_RYBN1",      EVT_mNAND_RYBN1),
		LPC313X_GPIO_PIN("mNAND_RYBN2",      EVT_mNAND_RYBN2),
		LPC313X_GPIO_PIN("mNAND_RYBN3",      EVT_mNAND_RYBN3),
		LPC313X_GPIO_PIN("mUART_CTS_N",      EVT_mUART_CTS_N),
		LPC313X_GPIO_PIN("mUART_RTS_N",      EVT_mUART_RTS_N),
		LPC313X_GPIO_PIN("mI2STX_DATA0",     EVT_mI2STX_DATA0),
		LPC313X_GPIO_PIN("mI2STX_WS0",       EVT_mI2STX_WS0),
		LPC313X_GPIO_PIN("EBI_NRAS_BLOUT_1", EVT_EBI_NRAS_BLOUT_1),
		LPC313X_GPIO_PIN("EBI_A_0_ALE",      EVT_EBI_A_0_ALE),
		LPC313X_GPIO_PIN("EBI_NWE",          EVT_EBI_NWE),
};

static struct lpc313x_gpio_pin gpio_pins_spi[] = {
		LPC313X_GPIO_PIN("SPI_MISO",    EVT_SPI_MISO),
		LPC313X_GPIO_PIN("SPI_MOSI",    EVT_SPI_MOSI),
		LPC313X_GPIO_PIN("SPI_CS_IN",   EVT_SPI_CS_IN),
		LPC313X_GPIO_PIN("SPI_SCK",     EVT_SPI_SCK),
		LPC313X_GPIO_PIN("SPI_CS_OUT0", EVT_SPI_CS_OUT0),
};

static struct lpc313x_gpio_pin gpio_pins_uart[] = {
		LPC313X_GPIO_PIN("UART_RXD", EVT_UART_RXD),
		LPC313X_GPIO_PIN("UART_TXD", EVT_UART_TXD),
};

static struct lpc313x_gpio_chip gpio_chips[] = {
		/* first chip is required for fast gpio support (XXX explain in detail) */
		LPC313X_GPIO_CHIP("GPIO",        IOCONF_GPIO,         0, 15, &gpio_pins_gpio),
		LPC313X_GPIO_CHIP("EBI_MCI",     IOCONF_EBI_MCI,     15, 32, &gpio_pins_ebi_mci),
		LPC313X_GPIO_CHIP("EBI_I2STX_0", IOCONF_EBI_I2STX_0, 47, 10, &gpio_pins_ebi_i2stx_0),
		LPC313X_GPIO_CHIP("CGU",         IOCONF_CGU,         57,  1, NULL),
		LPC313X_GPIO_CHIP("I2SRX_0",     IOCONF_I2SRX_0,     58,  3, NULL),
		LPC313X_GPIO_CHIP("I2SRX_1",     IOCONF_I2SRX_1,     61,  3, NULL),
		LPC313X_GPIO_CHIP("I2STX_1",     IOCONF_I2STX_1,     64,  4, NULL),
		LPC313X_GPIO_CHIP("EBI",         IOCONF_EBI,         68, 16, NULL),
		LPC313X_GPIO_CHIP("I2C1",        IOCONF_I2C1,        84,  2, NULL),
		LPC313X_GPIO_CHIP("SPI",         IOCONF_SPI,         86,  5, &gpio_pins_spi),
		LPC313X_GPIO_CHIP("NAND_CTRL",   IOCONF_NAND_CTRL,   91,  4, NULL),
		LPC313X_GPIO_CHIP("PWM",         IOCONF_PWM,         95,  1, NULL),
		LPC313X_GPIO_CHIP("UART",        IOCONF_UART,        96,  2, &gpio_pins_uart),
};

static int lpc313x_gpiolib_request(struct gpio_chip *chip, unsigned offset)
{
	return lpc313x_gpiolib_direction_input(chip, offset);
}

static void lpc313x_gpiolib_free(struct gpio_chip *chip, unsigned offset)
{
	struct lpc313x_gpio_chip *pchip = to_lpc313x_gpio_chip(chip);
	unsigned long flags;
	unsigned port = ((unsigned)pchip->regbase);
	unsigned pin = (1 << offset);

	raw_local_irq_save(flags);

	GPIO_M0_RESET(port) = pin;
	GPIO_M1_SET(port) = pin;

	raw_local_irq_restore(flags);
}

static int lpc313x_gpiolib_direction_input(struct gpio_chip *chip,
					   unsigned offset)
{
	struct lpc313x_gpio_chip *pchip = to_lpc313x_gpio_chip(chip);
	unsigned long flags;
	unsigned port = ((unsigned)pchip->regbase);
	unsigned pin = (1 << offset);

	raw_local_irq_save(flags);

	GPIO_M1_RESET(port) = pin;
	GPIO_M0_RESET(port) = pin;

	raw_local_irq_restore(flags);

	return 0;
}

static int lpc313x_gpiolib_direction_output(struct gpio_chip *chip,
					    unsigned offset,
					    int value)
{
	struct lpc313x_gpio_chip *pchip = to_lpc313x_gpio_chip(chip);
	unsigned long flags;
	unsigned port = ((unsigned)pchip->regbase);
	unsigned pin = (1 << offset);

	raw_local_irq_save(flags);

	GPIO_M1_SET(port) = pin;

	if(value) {
		GPIO_M0_SET(port) = pin;
	} else {
		GPIO_M0_RESET(port) = pin;
	}

	raw_local_irq_restore(flags);

	return 0;
}

static int lpc313x_gpiolib_get(struct gpio_chip *chip, unsigned offset)
{
	struct lpc313x_gpio_chip *pchip = to_lpc313x_gpio_chip(chip);
	unsigned port = ((unsigned)pchip->regbase);
	unsigned pin = (1 << offset);

	if(GPIO_STATE(port) & pin) {
		return 1;
	} else {
		return 0;
	}
}

static void lpc313x_gpiolib_set(struct gpio_chip *chip,
				unsigned offset,
				int value)
{
	struct lpc313x_gpio_chip *pchip = to_lpc313x_gpio_chip(chip);
	unsigned long flags;
	unsigned port = ((unsigned)pchip->regbase);
	unsigned pin = (1 << offset);

	raw_local_irq_save(flags);

	GPIO_M1_SET(port) = pin;

	if(value) {
		GPIO_M0_SET(port) = pin;
	} else {
		GPIO_M0_RESET(port) = pin;
	}

	raw_local_irq_restore(flags);
}

static int lpc313x_gpiolib_to_irq(struct gpio_chip *chip, unsigned offset)
{
	struct lpc313x_gpio_chip *pchip = to_lpc313x_gpio_chip(chip);

	if(pchip->pins) {
		return IRQ_FOR_EVENT(pchip->pins[offset].event_id);
	}

	return -1;
}

static void lpc313x_gpiolib_dbg_show(struct seq_file *s, struct gpio_chip *chip)
{
	struct lpc313x_gpio_chip *pchip = to_lpc313x_gpio_chip(chip);
	unsigned long flags;
	unsigned port = ((unsigned)pchip->regbase);
	unsigned pin, mode0, mode1, state;

	int i;

	for (i = 0; i < chip->ngpio; i++) {
		const char *gpio_label;

		pin = (1 << i);

		gpio_label = gpiochip_is_requested(chip, i);

		if (gpio_label) {

			raw_local_irq_save(flags);
			mode0 = GPIO_STATE_M0(port) & pin;
			mode1 = GPIO_STATE_M1(port) & pin;
			state = GPIO_STATE(port) & pin;
			raw_local_irq_restore(flags);

			seq_printf(s, "[%s] ", gpio_label);
			if(pchip->pins) {
				seq_printf(s, "%s: ", pchip->pins[i].name);
			} else {
				seq_printf(s, "%s[%d]: ", chip->label, i);
			}

			if(mode1) {
				seq_printf(s, "output %s\n", mode0 ? "high" : "low");
			} else {
				if(mode0) {
					seq_printf(s, "device function\n");
				} else {
					seq_printf(s, "input %s\n", state ? "high" : "low");
				}
			}
		}
	}
}

void __init lpc313x_gpiolib_init()
{
	int numchips = sizeof(gpio_chips) / sizeof(struct lpc313x_gpio_chip);
	int i;

	for(i = 0; i < numchips; i++) {
		gpiochip_add(&gpio_chips[i].chip);
	}
}
