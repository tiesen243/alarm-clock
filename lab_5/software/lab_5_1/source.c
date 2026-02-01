/*
 * source.c
 *
 *  Created on: Jan 28, 2026
 *  Author: tiesen243
 */

#include "altera_avalon_timer_regs.h"
#include "sys/alt_irq.h"
#include "system.h"
#include <stdio.h>

unsigned int counter = 0;

void Timer_IQR_Handler(void *isr_context) {
  counter++;
  printf("%d seconds\n", counter);

  IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE,
                                  ALTERA_AVALON_TIMER_STATUS_TO_MSK);
}

void Timer_Init(void) {
  unsigned int period = 0;
  IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,
                                   ALTERA_AVALON_TIMER_CONTROL_STOP_MSK);

  period = 50000000 - 1;
  IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, period);
  IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, period >> 16);

  IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,
                                   ALTERA_AVALON_TIMER_CONTROL_CONT_MSK |
                                       ALTERA_AVALON_TIMER_CONTROL_ITO_MSK |
                                       ALTERA_AVALON_TIMER_CONTROL_START_MSK);
}

int main(void) {
  Timer_Init();

  alt_ic_isr_register(0, TIMER_0_IRQ, Timer_IQR_Handler, (void *)0, (void *)0);

  while (1)
    ;

  return 0;
}
