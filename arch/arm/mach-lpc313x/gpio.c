/*  linux/arch/arm/mach-lpc313x/gpio.c
 *
 *  Author:	Durgesh Pattamatta
 *  Copyright (C) 2009 NXP semiconductors
 *
 * GPIO driver for LPC313x & LPC315x.
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
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>

#include <asm/errno.h>
#include <mach/hardware.h>
#include <mach/gpio.h>


void lpx313x_gpio_func_mode(int gpio)
{
	unsigned long flags;
	int port = (gpio & GPIO_PORT_MASK);
	int pin = 1 << (gpio & GPIO_PIN_MASK);

	raw_local_irq_save(flags);

	GPIO_M1_RESET(port) = pin; 
	GPIO_M0_SET(port) = pin;

	raw_local_irq_restore(flags);

}

EXPORT_SYMBOL(lpx313x_gpio_func_mode);




int lpc313x_gpio_direction_output(unsigned gpio, int value)
{
	unsigned long flags;
	int port = (gpio & GPIO_PORT_MASK);
	int pin = 1 << (gpio & GPIO_PIN_MASK);

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

EXPORT_SYMBOL(lpc313x_gpio_direction_output);
