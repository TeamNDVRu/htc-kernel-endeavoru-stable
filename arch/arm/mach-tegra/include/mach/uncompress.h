/*
 * arch/arm/mach-tegra/include/mach/uncompress.h
 *
 * Copyright (C) 2010 Google, Inc.
 *
 * Author:
 *	Colin Cross <ccross@google.com>
 *	Erik Gilling <konkers@google.com>
 *
 * Copyright (C) 2010-2011 NVIDIA Corporation
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __MACH_TEGRA_UNCOMPRESS_H
#define __MACH_TEGRA_UNCOMPRESS_H

#include <linux/types.h>
#include <linux/serial_reg.h>

#include <mach/iomap.h>

#if defined(CONFIG_TEGRA_DEBUG_UARTA)
#define DEBUG_UART_CLK_SRC		(TEGRA_CLK_RESET_BASE + 0x178)
#define DEBUG_UART_CLK_ENB_SET_REG	(TEGRA_CLK_RESET_BASE + 0x320)
#define DEBUG_UART_CLK_ENB_SET_BIT	(1 << 6)
#define DEBUG_UART_RST_CLR_REG		(TEGRA_CLK_RESET_BASE + 0x304)
#define DEBUG_UART_RST_CLR_BIT		(1 << 6)
#elif defined(CONFIG_TEGRA_DEBUG_UARTB)
#define DEBUG_UART_CLK_SRC		(TEGRA_CLK_RESET_BASE + 0x17c)
#define DEBUG_UART_CLK_ENB_SET_REG	(TEGRA_CLK_RESET_BASE + 0x320)
#define DEBUG_UART_CLK_ENB_SET_BIT	(1 << 7)
#define DEBUG_UART_RST_CLR_REG		(TEGRA_CLK_RESET_BASE + 0x304)
#define DEBUG_UART_RST_CLR_BIT		(1 << 7)
#elif defined(CONFIG_TEGRA_DEBUG_UARTC)
#define DEBUG_UART_CLK_SRC		(TEGRA_CLK_RESET_BASE + 0x1a0)
#define DEBUG_UART_CLK_ENB_SET_REG	(TEGRA_CLK_RESET_BASE + 0x328)
#define DEBUG_UART_CLK_ENB_SET_BIT	(1 << 23)
#define DEBUG_UART_RST_CLR_REG		(TEGRA_CLK_RESET_BASE + 0x30C)
#define DEBUG_UART_RST_CLR_BIT		(1 << 23)
#elif defined(CONFIG_TEGRA_DEBUG_UARTD)
#define DEBUG_UART_CLK_SRC		(TEGRA_CLK_RESET_BASE + 0x1c0)
#define DEBUG_UART_CLK_ENB_SET_REG	(TEGRA_CLK_RESET_BASE + 0x330)
#define DEBUG_UART_CLK_ENB_SET_BIT	(1 << 1)
#define DEBUG_UART_RST_CLR_REG		(TEGRA_CLK_RESET_BASE + 0x314)
#define DEBUG_UART_RST_CLR_BIT		(1 << 1)
#elif defined(CONFIG_TEGRA_DEBUG_UARTE)
#define DEBUG_UART_CLK_SRC		(TEGRA_CLK_RESET_BASE + 0x1c4)
#define DEBUG_UART_CLK_ENB_SET_REG	(TEGRA_CLK_RESET_BASE + 0x330)
#define DEBUG_UART_CLK_ENB_SET_BIT	(1 << 2)
#define DEBUG_UART_RST_CLR_REG		(TEGRA_CLK_RESET_BASE + 0x314)
#define DEBUG_UART_RST_CLR_BIT		(1 << 2)
#else
#define DEBUG_UART_CLK_SRC		0
#define DEBUG_UART_CLK_ENB_SET_REG	0
#define DEBUG_UART_CLK_ENB_SET_BIT	0
#define DEBUG_UART_RST_CLR_REG		0
#define DEBUG_UART_RST_CLR_BIT		0
#endif

#ifdef CONFIG_ARCH_TEGRA_2x_SOC
#define DEBUG_UART_DLL			0x75
#else
#define DEBUG_UART_DLL			0xdd
#endif

static void putc(int c)
{
	volatile u8 *uart = (volatile u8 *)TEGRA_DEBUG_UART_BASE;
	int shift = 2;

	if (uart == NULL)
		return;

	while (!(uart[UART_LSR << shift] & UART_LSR_THRE))
		barrier();
	uart[UART_TX << shift] = c;
}

static inline void flush(void)
{
}

static inline void konk_delay(int delay)
{
	int i;

	for (i = 0; i < (1000 * delay); i++) {
		barrier();
	}
}


static inline void arch_decomp_setup(void)
{
	volatile u8 *uart = (volatile u8 *)TEGRA_DEBUG_UART_BASE;
	int shift = 2;
	volatile u32 *addr;

	if (uart == NULL)
		return;

	/* Debug UART clock source is PLLP_OUT0. */
	addr = (volatile u32 *)DEBUG_UART_CLK_SRC;
	*addr = 0;

	/* Enable clock to debug UART. */
	addr = (volatile u32 *)DEBUG_UART_CLK_ENB_SET_REG;
	*addr = DEBUG_UART_CLK_ENB_SET_BIT;

	konk_delay(5);

	/* Deassert reset to debug UART. */
	addr = (volatile u32 *)DEBUG_UART_RST_CLR_REG;
	*addr = DEBUG_UART_RST_CLR_BIT;

	konk_delay(5);

	/* Set up debug UART. */
	uart[UART_LCR << shift] |= UART_LCR_DLAB;
	uart[UART_DLL << shift] = DEBUG_UART_DLL;
	uart[UART_DLM << shift] = 0x0;
	uart[UART_LCR << shift] = 3;
}

static inline void arch_decomp_wdog(void)
{
}

#endif
