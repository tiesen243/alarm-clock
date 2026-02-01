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

unsigned int counter = 1859;
unsigned int prev_button_state = 0;

const int HEX_7SEG[16] = {
    0x40, // 0
    0x79, // 1
    0x24, // 2
    0x30, // 3
    0x19, // 4
    0x12, // 5
    0x02, // 6
    0x78, // 7
    0x00, // 8
    0x10, // 9
};

void Timer_IQR_Handler(void *isr_context) {
  unsigned int switch_state = IORD(SWITCH_BASE, 0);
  unsigned int button_state = IORD(BUTTON_BASE, 0);

  if (switch_state != 1 && switch_state != 2) {
    counter++;
    if ((counter / 60) == 59 && (counter % 60) == 59)
      counter = 0;
  }

  if (button_state && !prev_button_state) {
    if (switch_state == 1) {
      counter++;
      if ((counter / 60) == 59 && (counter % 60) == 59)
        counter = 0;
    } else if (switch_state == 2) {
      counter += 60;
      if ((counter / 60) > 59)
        counter = counter % 60;
    }
  }
  prev_button_state = button_state;

  printf("%d seconds\n", counter);

  unsigned int seconds = counter % 60;
  unsigned int minutes = counter / 60;

  unsigned int sec_ones = seconds % 10;
  unsigned int sec_tens = seconds / 10;
  unsigned int min_ones = minutes % 10;
  unsigned int min_tens = minutes / 10;

  IOWR(HEX_0_BASE, 0, HEX_7SEG[min_tens]);
  IOWR(HEX_1_BASE, 0, HEX_7SEG[min_ones]);
  IOWR(HEX_2_BASE, 0, HEX_7SEG[sec_tens]);
  IOWR(HEX_3_BASE, 0, HEX_7SEG[sec_ones]);

  IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE,
                                  ALTERA_AVALON_TIMER_STATUS_TO_MSK);
}

// void Button_ISR_Handler(void* isr_context) {
//   const switch_state = IORD(SWITCH_BASE, 0);
//
//   if (switch_state == 1) {
//     counter++;
//     if ((counter / 60) == 99 && (counter % 60) == 59) counter = 0;
//   } else if (switch_state == 2) {
//     counter += 60;
//     if ((counter / 60) > 99) counter = counter % 60;
//   }
//
//   IOWR(BUTTON_BASE, 0, 0x0);
// }

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
  // alt_ic_isr_register(0, BUTTON_IRQ, Button_ISR_Handler, (void*) 0, (void*)
  // 0);

  while (1)
    ;

  return 0;
}
