/*
 * source.c
 *
 *  Created on: Jan 9, 2026
 *  Author: tiesen243
 */

#include <io.h>
#include <system.h>

int main() {
  int temp;
  while (1) {
    temp = IORD(SWITCH_BASE, 0) & 0x0F;
    IOWR(LED_BASE, 0, ((temp & 0x01) >> 0) & ((temp & 0x02) >> 1));
  }

  return 0;
}
