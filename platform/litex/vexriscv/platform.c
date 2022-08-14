/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Dolu1990 <charles.papon.90@gmail.com>
 *
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_const.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_system.h>
#include <sbi_utils/ipi/aclint_mswi.h>
#include <sbi_utils/irqchip/plic.h>
#include <sbi_utils/serial/litex-uart.h>
#include <sbi_utils/timer/aclint_mtimer.h>


/* clang-format off */

#define VEX_HART_COUNT		8
#define VEX_CLINT_ADDR		0xF0010000U
#define VEX_ACLINT_MTIMER_FREQ	1000000
#define VEX_ACLINT_MSWI_ADDR	(VEX_CLINT_ADDR + CLINT_MSWI_OFFSET)
#define VEX_ACLINT_MTIMER_ADDR	(VEX_CLINT_ADDR + CLINT_MTIMER_OFFSET)

#define VEX_CSR_BASE		0xF0000000U
#define VEX_UART_BASE		(VEX_CSR_BASE + 0x1000U)

/* clang-format on */

static struct aclint_mswi_data mswi = {
	.addr = VEX_ACLINT_MSWI_ADDR,
	.size = ACLINT_MSWI_SIZE,
	.first_hartid = 0,
	.hart_count = VEX_HART_COUNT,
};

static struct aclint_mtimer_data mtimer = {
	.mtime_freq = VEX_ACLINT_MTIMER_FREQ,
	.mtime_addr = VEX_ACLINT_MTIMER_ADDR +
		      ACLINT_DEFAULT_MTIME_OFFSET,
	.mtime_size = ACLINT_DEFAULT_MTIME_SIZE,
	.mtimecmp_addr = VEX_ACLINT_MTIMER_ADDR +
			 ACLINT_DEFAULT_MTIMECMP_OFFSET,
	.mtimecmp_size = ACLINT_DEFAULT_MTIMECMP_SIZE,
	.first_hartid = 0,
	.hart_count = VEX_HART_COUNT,
	.has_64bit_mmio = TRUE,
};

static int vex_early_init(bool cold_boot)
{
	return 0;
}

static int vex_final_init(bool cold_boot)
{
	return 0;
}

static int vex_console_init(void)
{
	return litex_uart_init(VEX_UART_BASE);
}

static int vex_irqchip_init(bool cold_boot)
{
	return 0;
}

static int vex_ipi_init(bool cold_boot)
{
	int rc;

	if (cold_boot) {
		rc = aclint_mswi_cold_init(&mswi);
		if (rc)
			return rc;
	}

	return aclint_mswi_warm_init();
}

static int vex_timer_init(bool cold_boot)
{
	int ret;

	if (cold_boot) {
		ret = aclint_mtimer_cold_init(&mtimer, NULL);
		if (ret)
			return ret;
	}

	return aclint_mtimer_warm_init();
}

const struct sbi_platform_operations platform_ops = {
	.early_init		= vex_early_init,
	.final_init		= vex_final_init,
	.console_init		= vex_console_init,
	.irqchip_init		= vex_irqchip_init,
	.ipi_init		= vex_ipi_init,
	.timer_init		= vex_timer_init,
};

const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x01),
	.name			= "LiteX / VexRiscv-SMP",
	.features		= SBI_PLATFORM_DEFAULT_FEATURES,
	.hart_count		= VEX_HART_COUNT,
	.hart_stack_size	= SBI_PLATFORM_DEFAULT_HART_STACK_SIZE,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
