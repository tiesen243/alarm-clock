/*
 * source.c
 *
 *  Created on: Jan 14, 2026
 *  Author: tiesen
 */

#include <system.h>

int main() {
  volatile short *LED_ptr = (short *)LED_BASE;
  volatile short *SW_ptr = (short *)SWITCH_BASE;

  short tmp;

  while (1) {
    tmp = *SW_ptr & 0xFF;
    *LED_ptr = tmp;
  }

  return 0;
}
