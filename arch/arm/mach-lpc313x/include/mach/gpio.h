/* linux/arch/arm/mach-lpc313x/include/mach/gpio.h
 *  
 *  Author:	Durgesh Pattamatta
 *  Copyright (C) 2009 NXP semiconductors
 *
 * GPIO defines & routines for LPC313x and LPC315x SoCs.
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
#ifndef _LPC313X_GPIO_H
#define _LPC313X_GPIO_H

#include <mach/hardware.h>

#if 0

#define GPIO_PORT_MASK  0x0FE0
#define GPIO_PIN_MASK   0x001F


#define GPIO_CGU_SYSCLK_O     (IOCONF_CGU | 0) 

#define GPIO_I2SRX_BCK0       (IOCONF_I2SRX_0 | 0) 
#define GPIO_I2SRX_DATA0      (IOCONF_I2SRX_0 | 1) 
#define GPIO_I2SRX_WS0        (IOCONF_I2SRX_0 | 2) 

#define GPIO_I2SRX_DATA1      (IOCONF_I2SRX_1 | 0) 
#define GPIO_I2SRX_BCK1       (IOCONF_I2SRX_1 | 1) 
#define GPIO_I2SRX_WS1        (IOCONF_I2SRX_1 | 2) 

#define GPIO_I2STX_DATA1      (IOCONF_I2STX_1 | 0) 
#define GPIO_I2STX_BCK1       (IOCONF_I2STX_1 | 1) 
#define GPIO_I2STX_WS1        (IOCONF_I2STX_1 | 2) 
#define GPIO_I2STX_256FS_O    (IOCONF_I2STX_1 | 3) 

#define GPIO_EBI_D_9          (IOCONF_EBI | 0)  
#define GPIO_EBI_D_10         (IOCONF_EBI | 1)  
#define GPIO_EBI_D_11         (IOCONF_EBI | 2)  
#define GPIO_EBI_D_12         (IOCONF_EBI | 3)  
#define GPIO_EBI_D_13         (IOCONF_EBI | 4)  
#define GPIO_EBI_D_14         (IOCONF_EBI | 5)  
#define GPIO_EBI_D_4          (IOCONF_EBI | 6)  
#define GPIO_EBI_D_0          (IOCONF_EBI | 7)  
#define GPIO_EBI_D_1          (IOCONF_EBI | 8)  
#define GPIO_EBI_D_2          (IOCONF_EBI | 9)  
#define GPIO_EBI_D_3          (IOCONF_EBI | 10) 
#define GPIO_EBI_D_5          (IOCONF_EBI | 11) 
#define GPIO_EBI_D_6          (IOCONF_EBI | 12) 
#define GPIO_EBI_D_7          (IOCONF_EBI | 13) 
#define GPIO_EBI_D_8          (IOCONF_EBI | 14) 
#define GPIO_EBI_D_15         (IOCONF_EBI | 15) 

#define GPIO_I2C_SDA1         (IOCONF_I2C1 | 0) 
#define GPIO_I2C_SCL1         (IOCONF_I2C1 | 1) 

#define GPIO_NAND_NCS_3       (IOCONF_NAND_CTRL | 0)
#define GPIO_NAND_NCS_0       (IOCONF_NAND_CTRL | 1)
#define GPIO_NAND_NCS_1       (IOCONF_NAND_CTRL | 2)
#define GPIO_NAND_NCS_2       (IOCONF_NAND_CTRL | 3)

#define GPIO_PWM_DATA         (IOCONF_PWM | 0)

#define GPIO_UART_RXD         (IOCONF_UART | 0)
#define GPIO_UART_TXD         (IOCONF_UART | 1)

#endif


#define BASE_GPIO_GPIO 0
#define NUM_GPIO_GPIO 15

#define GPIO_GPIO1  (BASE_GPIO_GPIO + 0)
#define GPIO_GPIO0  (BASE_GPIO_GPIO + 1)
#define GPIO_GPIO2  (BASE_GPIO_GPIO + 2)
#define GPIO_GPIO3  (BASE_GPIO_GPIO + 3)
#define GPIO_GPIO4  (BASE_GPIO_GPIO + 4)
#define GPIO_GPIO11 (BASE_GPIO_GPIO + 5)
#define GPIO_GPIO12 (BASE_GPIO_GPIO + 6)
#define GPIO_GPIO13 (BASE_GPIO_GPIO + 7)
#define GPIO_GPIO14 (BASE_GPIO_GPIO + 8)
#define GPIO_GPIO15 (BASE_GPIO_GPIO + 9)
#define GPIO_GPIO16 (BASE_GPIO_GPIO + 10)
#define GPIO_GPIO17 (BASE_GPIO_GPIO + 11)
#define GPIO_GPIO18 (BASE_GPIO_GPIO + 12)
#define GPIO_GPIO19 (BASE_GPIO_GPIO + 13)
#define GPIO_GPIO20 (BASE_GPIO_GPIO + 14)


#define BASE_GPIO_EBI_MCI 16
#define NUM_GPIO_EBI_MCI 32

#define GPIO_MGPIO9           (BASE_GPIO_EBI_MCI + 0)
#define GPIO_MGPIO6           (BASE_GPIO_EBI_MCI + 1)
#define GPIO_MLCD_DB_7        (BASE_GPIO_EBI_MCI + 2)
#define GPIO_MLCD_DB_4        (BASE_GPIO_EBI_MCI + 3)
#define GPIO_MLCD_DB_2        (BASE_GPIO_EBI_MCI + 4)
#define GPIO_MNAND_RYBN0      (BASE_GPIO_EBI_MCI + 5)
#define GPIO_MI2STX_CLK0      (BASE_GPIO_EBI_MCI + 6)
#define GPIO_MI2STX_BCK0      (BASE_GPIO_EBI_MCI + 7)
#define GPIO_EBI_A_1_CLE      (BASE_GPIO_EBI_MCI + 8)
#define GPIO_EBI_NCAS_BLOUT   (BASE_GPIO_EBI_MCI + 9)
#define GPIO_MLCD_DB_0        (BASE_GPIO_EBI_MCI + 10)
#define GPIO_EBI_DQM_0_NOE    (BASE_GPIO_EBI_MCI + 11)
#define GPIO_MLCD_CSB         (BASE_GPIO_EBI_MCI + 12)
#define GPIO_MLCD_DB_1        (BASE_GPIO_EBI_MCI + 13)
#define GPIO_MLCD_E_RD        (BASE_GPIO_EBI_MCI + 14)
#define GPIO_MLCD_RS          (BASE_GPIO_EBI_MCI + 15)
#define GPIO_MLCD_RW_WR       (BASE_GPIO_EBI_MCI + 16)
#define GPIO_MLCD_DB_3        (BASE_GPIO_EBI_MCI + 17)
#define GPIO_MLCD_DB_5        (BASE_GPIO_EBI_MCI + 18)
#define GPIO_MLCD_DB_6        (BASE_GPIO_EBI_MCI + 19)
#define GPIO_MLCD_DB_8        (BASE_GPIO_EBI_MCI + 20)
#define GPIO_MLCD_DB_9        (BASE_GPIO_EBI_MCI + 21)
#define GPIO_MLCD_DB_10       (BASE_GPIO_EBI_MCI + 22)
#define GPIO_MLCD_DB_11       (BASE_GPIO_EBI_MCI + 23)
#define GPIO_MLCD_DB_12       (BASE_GPIO_EBI_MCI + 24)
#define GPIO_MLCD_DB_13       (BASE_GPIO_EBI_MCI + 25)
#define GPIO_MLCD_DB_14       (BASE_GPIO_EBI_MCI + 26)
#define GPIO_MLCD_DB_15       (BASE_GPIO_EBI_MCI + 27)
#define GPIO_MGPIO5           (BASE_GPIO_EBI_MCI + 28)
#define GPIO_MGPIO7           (BASE_GPIO_EBI_MCI + 29)
#define GPIO_MGPIO8           (BASE_GPIO_EBI_MCI + 30)
#define GPIO_MGPIO10          (BASE_GPIO_EBI_MCI + 31)


#define BASE_GPIO_EBI_I2STX_0 47
#define NUM_GPIO_EBI_I2STX_0 10

#define GPIO_MNAND_RYBN1      (BASE_GPIO_EBI_I2STX_0 + 0)
#define GPIO_MNAND_RYBN2      (BASE_GPIO_EBI_I2STX_0 + 1)
#define GPIO_MNAND_RYBN3      (BASE_GPIO_EBI_I2STX_0 + 2)
#define GPIO_MUART_CTS_N      (BASE_GPIO_EBI_I2STX_0 + 3)
#define GPIO_MUART_RTS_N      (BASE_GPIO_EBI_I2STX_0 + 4)
#define GPIO_MI2STX_DATA0     (BASE_GPIO_EBI_I2STX_0 + 5)
#define GPIO_MI2STX_WS0       (BASE_GPIO_EBI_I2STX_0 + 6)
#define GPIO_EBI_NRAS_BLOUT   (BASE_GPIO_EBI_I2STX_0 + 7)
#define GPIO_EBI_A_0_ALE      (BASE_GPIO_EBI_I2STX_0 + 8)
#define GPIO_EBI_NWE          (BASE_GPIO_EBI_I2STX_0 + 9)


#define BASE_GPIO_SPI 86
#define NUM_GPIO_SPI 5

#define GPIO_SPI_MISO         (BASE_GPIO_SPI + 0)
#define GPIO_SPI_MOSI         (BASE_GPIO_SPI + 1)
#define GPIO_SPI_CS_IN        (BASE_GPIO_SPI + 2)
#define GPIO_SPI_SCK          (BASE_GPIO_SPI + 3)
#define GPIO_SPI_CS_OUT0      (BASE_GPIO_SPI + 4)


/* gpiolib init function */
extern void lpc313x_gpiolib_init(void);

/* gpiolib fastpath and aliases */

#define IOCONF_FAST_GPIO IOCONF_GPIO
#define NUM_FAST_GPIO 15

#include <asm-generic/gpio.h>

static inline int gpio_get_value(unsigned gpio)
{
	if(gpio < NUM_FAST_GPIO) {
		unsigned bit = (1 << gpio);
		if(GPIO_STATE(IOCONF_FAST_GPIO) & bit) {
			return 1;
		} else {
			return 0;
		}
	} else {
		return __gpio_get_value(gpio);
	}
}

static inline void gpio_set_value(unsigned gpio, int value)
{
	if(gpio < NUM_FAST_GPIO) {
		unsigned bit = (1 << gpio);
		if(value) {
			GPIO_M0_SET(IOCONF_FAST_GPIO) = bit;
		} else {
			GPIO_M0_RESET(IOCONF_FAST_GPIO) = bit;
		}
	} else {
		__gpio_set_value(gpio, value);
	}
}

static inline int gpio_cansleep(unsigned gpio)
{
	return __gpio_cansleep(gpio);
}

static inline int gpio_to_irq(unsigned gpio)
{
	return __gpio_to_irq(gpio);
}

#endif /*_LPC313X_GPIO_H*/
