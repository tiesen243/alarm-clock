/*
 * source.c
 *
 *  Created on: Jan 9, 2026
 *  Author: tiesen243
 */

#include <io.h>
#include <system.h>

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
    0x08, // A
    0x03, // b
    0x46, // C
    0x21, // d
    0x06, // E
    0x0E  // F
};

int main() {
  int sw;

  while (1) {
    sw = IORD(SWITCH_BASE, 0) & 0x0F;
    IOWR(LED_BASE, 0, HEX_7SEG[sw]);
  }

  return 0;
}
