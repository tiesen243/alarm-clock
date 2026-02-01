/*
 * source.c
 *
 *  Created on: Jan 9, 2026
 *  Author: tiesen243
 */

#include <stdio.h>

int main() {
  int a;
  int *b;
  int c;
  a = 3;
  b = &a;
  *b = 5;
  c = a + 4;

  return 0;
}
