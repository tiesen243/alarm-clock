/*
 * source.c
 *
 *  Created on: Jan 9, 2026
 *  Author: tiesen243
 */

#include <io.h>
#include <system.h>

int button;

int main() {
  while (1) {
    button = IORD(SWITCH_BASE, 0);
    IOWR(LED_BASE, 0, button);
  }

  return 0;
}
