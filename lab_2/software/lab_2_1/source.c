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
    temp = IORD(SWITCH_BASE, 0);
    IOWR(LED_BASE, 0, temp);
  }

  return 0;
}
