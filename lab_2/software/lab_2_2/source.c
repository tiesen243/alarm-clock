/*
 * source.c
 *
 *  Created on: Jan 9, 2026
 *  Author: tiesen243
 */

#include "system.h"

int main() {
  volatile int *switch_ptr = (int *)SWITCH_BASE;
  volatile int *led_ptr = (int *)LED_BASE;

  int temp;

  while (1) {
    temp = *switch_ptr;
    *led_ptr = temp;
  }

  return 0;
}
