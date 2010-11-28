/****************************************************************************
 * config/olimex-lpc1766stk/src/up_nsh.c
 * arch/arm/src/board/up_nsh.c
 *
 *   Copyright (C) 2010 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <spudmonkey@racsa.co.cr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdio.h>
#include <debug.h>
#include <errno.h>

#include <nuttx/spi.h>
#include <nuttx/mmcsd.h>

/****************************************************************************
 * Pre-Processor Definitions
 ****************************************************************************/

/* Configuration ************************************************************/

/* PORT and SLOT number probably depend on the board configuration */

#ifdef CONFIG_ARCH_BOARD_LPC1766STK
#  define CONFIG_EXAMPLES_NSH_HAVEMMCSD  1
#  if !defined(CONFIG_EXAMPLES_NSH_MMCSDSPIPORTNO) || CONFIG_EXAMPLES_NSH_MMCSDSPIPORTNO != 1
#    error "The LPC1766-STK MMC/SD is on SSP1"
#    undef CONFIG_EXAMPLES_NSH_MMCSDSPIPORTNO
#    define CONFIG_EXAMPLES_NSH_MMCSDSPIPORTNO 1
#  endif
#  if !defined(CONFIG_EXAMPLES_NSH_MMCSDSLOTNO) || CONFIG_EXAMPLES_NSH_MMCSDSLOTNO != 0
#    error "The LPC1766-STK MMC/SD is only one slot (0)"
#    undef CONFIG_EXAMPLES_NSH_MMCSDSLOTNO
#    define CONFIG_EXAMPLES_NSH_MMCSDSLOTNO 0
#  endif
#  ifndef CONFIG_LPC17_SSP1
#    warning "CONFIG_LPC17_SSP1 is not enabled"
#  endif
#else
#  error "Unrecognized board"
#  undef CONFIG_EXAMPLES_NSH_HAVEMMCSD
#endif

/* Can't support MMC/SD features if mountpoints are disabled */

#if defined(CONFIG_DISABLE_MOUNTPOINT)
#  undef CONFIG_EXAMPLES_NSH_HAVEMMCSD
#endif

#ifndef CONFIG_EXAMPLES_NSH_MMCSDMINOR
#  define CONFIG_EXAMPLES_NSH_MMCSDMINOR 0
#endif

/* Debug ********************************************************************/

#ifdef CONFIG_CPP_HAVE_VARARGS
#  ifdef CONFIG_DEBUG
#    define message(...) lib_lowprintf(__VA_ARGS__)
#  else
#    define message(...) printf(__VA_ARGS__)
#  endif
#else
#  ifdef CONFIG_DEBUG
#    define message lib_lowprintf
#  else
#    define message printf
#  endif
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: nsh_archinitialize
 *
 * Description:
 *   Perform architecture specific initialization
 *
 ****************************************************************************/

int nsh_archinitialize(void)
{
  FAR struct spi_dev_s *ssp;
  int ret;

  /* Get the SSP port */

  ssp = up_spiinitialize(CONFIG_EXAMPLES_NSH_MMCSDSPIPORTNO);
  if (!ssp)
    {
      message("nsh_archinitialize: Failed to initialize SSP port %d\n",
              CONFIG_EXAMPLES_NSH_MMCSDSPIPORTNO);
      return -ENODEV;
    }

  message("Successfully initialized SSP port %d\n",
          CONFIG_EXAMPLES_NSH_MMCSDSPIPORTNO);

  /* Bind the SSP port to the slot */

  ret = mmcsd_spislotinitialize(CONFIG_EXAMPLES_NSH_MMCSDMINOR, CONFIG_EXAMPLES_NSH_MMCSDSLOTNO, ssp);
  if (ret < 0)
    {
      message("nsh_archinitialize: Failed to bind SSP port %d to MMC/SD slot %d: %d\n",
              CONFIG_EXAMPLES_NSH_MMCSDSPIPORTNO, CONFIG_EXAMPLES_NSH_MMCSDSLOTNO, ret);
      return ret;
    }

  message("Successfuly bound SSP port %d to MMC/SD slot %d\n",
          CONFIG_EXAMPLES_NSH_MMCSDSPIPORTNO, CONFIG_EXAMPLES_NSH_MMCSDSLOTNO);
  return OK;
}
