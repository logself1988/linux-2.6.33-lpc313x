/* linux/arch/arm/mach-lpc313x/include/mach/irqs.h
 *
 *  Author:	Durgesh Pattamatta
 *  Copyright (C) 2009 NXP semiconductors
 *
 *  IRQ defines for LPC313x and LPC315x SoCs.
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


#ifndef __ASM_ARCH_IRQS_H
#define __ASM_ARCH_IRQS_H

#include "event_router.h"

#define IRQ_CPU_START 1

#define IRQ_EVT_ROUTER0  1        /*interrupts from Event router 0*/
#define IRQ_EVT_ROUTER1  2        /*interrupts from Event router 1*/
#define IRQ_EVT_ROUTER2  3        /*interrupts from Event router 2*/
#define IRQ_EVT_ROUTER3  4        /*interrupts from Event router 3*/
#define IRQ_TIMER0       5        /*Timer 0 IRQ */
#define IRQ_TIMER1       6        /*Timer 1 IRQ */
#define IRQ_TIMER2       7        /*Timer 2 IRQ */
#define IRQ_TIMER3       8        /*Timer 3 IRQ */
#define IRQ_ADC          9        /*10bit ADC irq*/
#define IRQ_UART         10       /*UART irq */
#define IRQ_I2C0         11       /*I2C 0 IRQ */
#define IRQ_I2C1         12       /*I2C 1 IRQ */
#define IRQ_I2S0_OUT     13       /*I2S 0 out IRQ */
#define IRQ_I2S1_OUT     14       /*I2S 1 out IRQ */
#define IRQ_I2S0_IN      15       /*I2S 0 IN IRQ */
#define IRQ_I2S1_IN      16       /*I2S 1 IN IRQ */
#define IRQ_LCD          18       /*LCD irq */
#define IRQ_SPI_SMS      19       /*SPI SMS IRQ */
#define IRQ_SPI_TX       20       /*SPI Transmit IRQ */
#define IRQ_SPI_RX       21       /*SPI Receive IRQ */
#define IRQ_SPI_OVR      22       /*SPI overrun IRQ */
#define IRQ_SPI          23       /*SPI interrupt IRQ */
#define IRQ_DMA          24       /*DMA irq */
#define IRQ_NAND_FLASH   25       /*NAND flash irq */
#define IRQ_MCI          26       /*MCI irq */
#define IRQ_USB          27       /*USB irq */
#define IRQ_ISRAM0       28       /*ISRAM0 irq */
#define IRQ_ISRAM1       29       /*ISRAM1 irq */

#define NR_IRQ_CPU	  30	/* IRQs directly recognized by CPU */


#define IRQ_EVT_START 30

#define IRQ_FOR_EVENT(evt) (evt + NR_IRQ_CPU)
#define EVENT_FOR_IRQ(irq) (irq - NR_IRQ_CPU)

#define NR_IRQ_EVT NR_EVT

#define IRQ_EVT_END (IRQ_EVT_START + NR_IRQ_EVT)


#define NR_IRQS (NR_IRQ_CPU + NR_IRQ_EVT)


#define IRQ_WDT       IRQ_FOR_EVENT(EVT_wdog_m0)
#define IRQ_VBUS_EN   IRQ_FOR_EVENT(EVT_usb_otg_vbus_pwr_en)
#define IRQ_VBUS_OVRC IRQ_FOR_EVENT(EVT_USB_VBUS)
#define IRQ_USB_ID    IRQ_FOR_EVENT(EVT_USB_ID)

#define _INTERNAL_IRQ_EVENT_MAP	\
	{IRQ_WDT, EVT_wdog_m0, EVT_RISING_EDGE}, \
	{IRQ_VBUS_EN, EVT_usb_otg_vbus_pwr_en, EVT_FALLING_EDGE}, \
	{IRQ_VBUS_OVRC, EVT_USB_VBUS, EVT_FALLING_EDGE}, \
	{IRQ_USB_ID, EVT_USB_ID, EVT_ACTIVE_LOW}, \


/* Chip-specific stuff (analog die) */
#if defined(CONFIG_LPC3152_AD)

#define AD_IRQ_EVENT_MAP	\
	{IRQ_RTC, EVT_AD_NINT_I, EVT_ACTIVE_LOW}, \
	{IRQ_PLAY, EVT_PLAY_DET_I, EVT_ACTIVE_HIGH}, \

#define CHIP_IRQ_EVENT_MAP   _INTERNAL_IRQ_EVENT_MAP \
	AD_IRQ_EVENT_MAP 

#else
#define CHIP_IRQ_EVENT_MAP   _INTERNAL_IRQ_EVENT_MAP
#endif


/* Board-specific magic legacy interrupts */
#if defined (CONFIG_MACH_VAL3153)
#define IRQ_CS8900_ETH_INT IRQ_FOR_EVENT(EVT_GPIO16)
#define IRQ_SDMMC_CD0 IRQ_FOR_EVENT(EVT_GPIO12)
#define IRQ_SDMMC_SD1 IRQ_FOR_EVENT(EVT_GPIO13)
#define BOARD_IRQ_EVENT_MAP	{ \
	CHIP_IRQ_EVENT_MAP \
	{IRQ_CS8900_ETH_INT, EVT_GPIO16, EVT_ACTIVE_HIGH}, \
	{IRQ_SDMMC_CD0, EVT_GPIO12, EVT_ACTIVE_HIGH}, \
	{IRQ_SDMMC_CD1, EVT_GPIO13, EVT_ACTIVE_HIGH}, \
	}
#elif defined (CONFIG_MACH_EA313X) || defined(CONFIG_MACH_EA3152)
#define IRQ_DM9000_ETH_INT IRQ_FOR_EVENT(EVT_mNAND_RYBN3)
#define IRQ_SDMMC_CD IRQ_FOR_EVENT(EVT_mI2STX_BCK0)
#define IRQ_EA_VBUS_OVRC IRQ_FOR_EVENT(EVT_I2SRX_WS0)
#define BOARD_IRQ_EVENT_MAP	{ \
	CHIP_IRQ_EVENT_MAP \
	{IRQ_DM9000_ETH_INT, EVT_mNAND_RYBN3, EVT_ACTIVE_HIGH}, \
	{IRQ_SDMMC_CD, EVT_mI2STX_BCK0, EVT_ACTIVE_LOW}, \
	{IRQ_EA_VBUS_OVRC, EVT_I2SRX_WS0, EVT_ACTIVE_LOW}, \
	}
#elif defined (CONFIG_MACH_VAL3154)
#define IRQ_SDMMC_CD IRQ_FOR_EVENT(EVT_mI2STX_BCK0)
#define BOARD_IRQ_EVENT_MAP	{ \
	CHIP_IRQ_EVENT_MAP \
	{IRQ_SDMMC_CD, EVT_mI2STX_BCK0, EVT_ACTIVE_LOW}, \
	}
#else
#define BOARD_IRQ_EVENT_MAP { \
	CHIP_IRQ_EVENT_MAP \
	}
#endif

#endif
