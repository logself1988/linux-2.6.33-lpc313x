/*  arch/arm/mach-lpc313x/time.c
 *
 *  Author:	Durgesh Pattamatta
 *  Copyright (C) 2009 NXP semiconductors
 *
 *  Timer driver for LPC313x & LPC315x.
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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/time.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>


#include <mach/hardware.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/leds.h>

#include <asm/mach/time.h>
#include <mach/gpio.h>
#include <mach/board.h>
//#include <mach/cgu.h>

struct lpc313x_timer {
	/* id of timer */
	int id;
	/* physical base */
	unsigned long phys_base;
	/* CGU clock id */
	int clk_id;
	/* irq number */
	int irq;

	char *descr;

	/* timer reserved for static use */
	unsigned reserved:1;
	/* timer currently allocated */
	unsigned used:1;
};

#define TIMER_IO_SIZE (SZ_1K)

struct lpc313x_timer timers[] = {
	{.phys_base = TIMER0_PHYS, .clk_id = CGU_SB_TIMER0_PCLK_ID, .irq = IRQ_TIMER0 },
	{.phys_base = TIMER1_PHYS, .clk_id = CGU_SB_TIMER1_PCLK_ID, .irq = IRQ_TIMER1 },
	{.phys_base = TIMER2_PHYS, .clk_id = CGU_SB_TIMER2_PCLK_ID, .irq = IRQ_TIMER2 },
	{.phys_base = TIMER3_PHYS, .clk_id = CGU_SB_TIMER3_PCLK_ID, .irq = IRQ_TIMER3 },
};

#define NUM_TIMERS (sizeof(timers)/sizeof(struct lpc313x_timer))

void lpc313x_generic_timer_init(void)
{
	int i;
	for(i = 0; i < NUM_TIMERS; i++) {
		struct lpc313x_timer *t = &timers[i];

		/* if timer is reserved, mark as used and ignore */
		if(t->reserved) {
			t->used = 1;
			continue;
		}

		/* enable the clock of the timer */
		cgu_clk_en_dis(t->clk_id, 1);

		/* initialize the timer itself */
		TIMER_CONTROL(t->phys_base) = 0;
		TIMER_CLEAR(t->phys_base) = 0;

		/* disable clock again, will be enabled when allocated */
		cgu_clk_en_dis(t->clk_id, 0);
	}
}

struct lpc313x_timer *lpc313x_generic_timer_request(char *descr)
{
	int i;
	struct lpc313x_timer *t = NULL;
	for(i = 0; i < NUM_TIMERS; i++) {
		t = &timers[i];

		if(t->used)
			continue;

		/* enable the clock of the timer */
		cgu_clk_en_dis(t->clk_id, 1);

		/* mark the timer as used */
		t->used = 1;

		/* attach description */
		t->descr = descr;

		break;
	}
	return t;
}

void lpc313x_generic_timer_free(struct lpc313x_timer *t)
{
	TIMER_CONTROL(t->phys_base) = 0;

	cgu_clk_en_dis(t->clk_id, 0);

	t->descr = NULL;
	t->used = 0;
}

int lpc313x_generic_timer_get_irq(struct lpc313x_timer *t)
{
	return t->irq;
}

void lpc313x_generic_timer_ack_irq(struct lpc313x_timer *t)
{
	TIMER_CLEAR(t->phys_base) = 0;
}

u32 lpc313x_generic_timer_get_infreq(struct lpc313x_timer *t)
{
	return cgu_get_clk_freq(t->clk_id);
}

void lpc313x_generic_timer_periodic(struct lpc313x_timer *t, u32 period)
{
	TIMER_CONTROL(t->phys_base) &= ~TM_CTRL_ENABLE;
	TIMER_LOAD(t->phys_base) = period;
	TIMER_CONTROL(t->phys_base) |= TM_CTRL_ENABLE | TM_CTRL_PERIODIC;
	TIMER_CLEAR(t->phys_base) = 0;
}

void lpc313x_generic_timer_continuous(struct lpc313x_timer *t)
{
	TIMER_CONTROL(t->phys_base) &= ~TM_CTRL_ENABLE;
	TIMER_CONTROL(t->phys_base) |= TM_CTRL_ENABLE;
	TIMER_CLEAR(t->phys_base) = 0;
}

void lpc313x_generic_timer_stop(struct lpc313x_timer *t)
{
	TIMER_CONTROL(t->phys_base) &= ~TM_CTRL_ENABLE;
}

void lpc313x_generic_timer_continue(struct lpc313x_timer *t)
{
	TIMER_CONTROL(t->phys_base) |= TM_CTRL_ENABLE;
}

#if defined (CONFIG_DEBUG_FS)
static int oncefoo = 0;

static int lpc313x_timers_show(struct seq_file *s, void *v)
{
	int i;
	for(i = 0; i < NUM_TIMERS; i++) {
		struct lpc313x_timer *t = &timers[i];
		int clken;
		u32 ctrl;

		if(t->reserved) {
			seq_printf(s, "timer%d is reserved\n", i);
		}
		if(t->used) {
			seq_printf(s, "timer%d is allocated as \"%s\"\n", i, t->descr);
		}

		clken = cgu_clk_is_enabled(t->clk_id);

		if(!clken) {
			seq_printf(s, "timer%d has clock disabled\n", i);
			continue;
		}

		seq_printf(s, "timer%d input clock running at %d Hz\n", i,
			   lpc313x_generic_timer_get_infreq(t));

		ctrl = TIMER_CONTROL(t->phys_base);

		seq_printf(s, "timer%d is %s in mode %s\n", i,
			   (ctrl & TM_CTRL_ENABLE)?"running":"stopped",
			   (ctrl & TM_CTRL_PERIODIC)?"periodic":"continuous");
		
		if(ctrl & TM_CTRL_PERIODIC) {
			seq_printf(s, "timer%d value 0x%08x load 0x%08x\n", i,
				   TIMER_VALUE(t->phys_base), TIMER_LOAD(t->phys_base));
		} else {
			seq_printf(s, "timer%d value 0x%08x\n", i,
				   TIMER_VALUE(t->phys_base));
		}
	}

	if(!oncefoo) {
		struct lpc313x_timer *p = lpc313x_generic_timer_request("periodic");
		lpc313x_generic_timer_periodic(p, 24000000);

		struct lpc313x_timer *c = lpc313x_generic_timer_request("continuous");
		lpc313x_generic_timer_continuous(c);

		oncefoo = 1;
	}

	return 0;
}

static int lpc313x_timers_open(struct inode *inode, struct file *file)
{
	return single_open(file, &lpc313x_timers_show, inode->i_private);
}

static const struct file_operations lpc313x_timers_fops = {
	.owner		= THIS_MODULE,
	.open		= lpc313x_timers_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

void lpc313x_timer_init_debugfs(void)
{
	struct dentry		*node;

	node = debugfs_create_file("timers", S_IRUSR, NULL, NULL,
			&lpc313x_timers_fops);
	if (IS_ERR(node)) {
		printk("lpc313x_timers_init: failed to init debugfs\n");
	}

	return;
}
#else
void lpc313x_timer_init_debugfs(void) {}
#endif


static irqreturn_t lpc313x_timer_interrupt(int irq, void *dev_id)
{
	struct lpc313x_timer *t = (struct lpc313x_timer *)dev_id;

	lpc313x_generic_timer_ack_irq(t);

	timer_tick();

	return IRQ_HANDLED;
}

static struct irqaction lpc313x_timer_irq = {
	.name		= "LPC313x Timer Tick",
	.flags		= IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
	.handler	= lpc313x_timer_interrupt,
};

static void __init lpc313x_timer_init (void)
{
	struct lpc313x_timer *t;

	lpc313x_generic_timer_init();

	t = lpc313x_generic_timer_request("tick");

	lpc313x_timer_irq.dev_id = t;

	lpc313x_generic_timer_periodic(t, LATCH);

	setup_irq (lpc313x_generic_timer_get_irq(t),
		   &lpc313x_timer_irq);
}


/*!
 * Returns number of us since last clock interrupt.  Note that interrupts
 * will have been disabled by do_gettimeoffset()
 */
static unsigned long lpc313x_gettimeoffset(void)
{
	u32 elapsed = LATCH - TIMER_VALUE(TIMER0_PHYS);
	return ((elapsed * 100) / (XTAL_CLOCK / 20000));
}

static void lpc313x_timer_suspend(void)
{
	TIMER_CONTROL(TIMER0_PHYS) &= ~TM_CTRL_ENABLE;	/* disable timers */
}

static void lpc313x_timer_resume(void)
{
	TIMER_CONTROL(TIMER0_PHYS) |= TM_CTRL_ENABLE;	/* enable timers */
}


struct sys_timer lpc313x_timer = {
	.init = lpc313x_timer_init,
	.offset = lpc313x_gettimeoffset,
	.suspend = lpc313x_timer_suspend,
	.resume = lpc313x_timer_resume,
};
