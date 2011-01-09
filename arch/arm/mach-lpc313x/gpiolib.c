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

#include <mach/registers.h>

struct lpc313x_gpio_chip {
	struct gpio_chip  chip;
	void __iomem     *regbase;
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
static void lpc313x_gpiolib_dbg_show(struct seq_file *s,
										struct gpio_chip *chip);

#define LPC313X_GPIO_CHIP(name, basereg, base_gpio, nr_gpio)      \
    {                                                             \
    .chip = {                                                     \
            .label            = name,                             \
            .request          = lpc313x_gpiolib_request,          \
            .free             = lpc313x_gpiolib_free,             \
            .direction_input  = lpc313x_gpiolib_direction_input,  \
            .direction_output = lpc313x_gpiolib_direction_output, \
            .get              = lpc313x_gpiolib_get,              \
            .set              = lpc313x_gpiolib_set,              \
            .dbg_show         = lpc313x_gpiolib_dbg_show,         \
            .base             = (base_gpio),                      \
            .ngpio            = (nr_gpio),                        \
        },                                                        \
    .regbase = ((void *)basereg),                                 \
    }

static struct lpc313x_gpio_chip gpio_chips[] = {
		/* first chip is required */
		LPC313X_GPIO_CHIP("GPIO",        IOCONF_GPIO,         0, 15),

		/* boards that need more should enable chips below */
#if 0
		LPC313X_GPIO_CHIP("EBI_MCI",     IOCONF_EBI_MCI,     15, 32),
		LPC313X_GPIO_CHIP("EBI_I2STX_0", IOCONF_EBI_I2STX_0, 47, 10),
		LPC313X_GPIO_CHIP("CGU",         IOCONF_CGU,         57,  1),
		LPC313X_GPIO_CHIP("I2SRX_0",     IOCONF_I2SRX_0,     58,  3),
		LPC313X_GPIO_CHIP("I2SRX_1",     IOCONF_I2SRX_1,     61,  3),
		LPC313X_GPIO_CHIP("I2STX_1",     IOCONF_I2STX_1,     64,  4),
		LPC313X_GPIO_CHIP("EBI",         IOCONF_EBI,         68, 16),
		LPC313X_GPIO_CHIP("I2C1",        IOCONF_I2C1,        84,  2),
		LPC313X_GPIO_CHIP("SPI",         IOCONF_SPI,         86,  5),
		LPC313X_GPIO_CHIP("NAND_CTRL",   IOCONF_NAND_CTRL,   91,  4),
		LPC313X_GPIO_CHIP("PWM",         IOCONF_PWM,         95,  1),
		LPC313X_GPIO_CHIP("UART",        IOCONF_UART,        96,  2),
#endif
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

			seq_printf(s, "[%s] %s%d: ", gpio_label, chip->label, i);

			if(mode0) {
				seq_printf(s, "output %s\n", mode1 ? "high" : "low");
			} else {
				if(mode1) {
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
