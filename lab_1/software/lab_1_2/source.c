/*
 * source.c
 *
 *  Created on: Feb 12, 2026
 *      Author: tiesen
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
