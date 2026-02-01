/*
 * source.c
 *
 *  Created on: Feb 1, 2026
 *      Author: tiesen243
 */

#include "altera_avalon_pio_regs.h"
#include "sys/alt_stdio.h"
#include "system.h"
#include <stdio.h>

int counter = 0;

void delay(int a) {
  volatile int i = 0;
  while (i < a * 100)
    i++;
}

void command(data) {
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_RS_BASE, 0x00);
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_RW_BASE, 0x00);
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_D_BASE, data & 0xFF);
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_EN_BASE, 0x01);
  delay(20);

  IOWR_ALTERA_AVALON_PIO_DATA(LCD_EN_BASE, 0x00);
  delay(20);
}

void lcd_data(char data) {
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_RS_BASE, 0x01);
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_RW_BASE, 0x00);
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_D_BASE, data & 0xFF);
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_EN_BASE, 0x01);
  delay(20);

  IOWR_ALTERA_AVALON_PIO_DATA(LCD_EN_BASE, 0x00);
  delay(20);
}

void lcd_string(char *str) {
  int i = 0;
  while (str[i] != 0) {
    lcd_data(str[i]);
    i++;
  }
}

void lcd_init() {
  command(0x38);
  delay(100);
  command(0x0c);
  delay(100);
  command(0x06);
  delay(100);
  command(0x01);
}

int main() {
  char data[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

  lcd_init();
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_ON_BASE, 0x01);
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_BLON_BASE, 0x01);

  while (1) {
    if (IORD(BUTTON_BASE, 0) == 0) {
      while (IORD(BUTTON_BASE, 0) == 0)
        ;
      counter++;
      if (counter > 99)
        counter = 0;
    }

    command(0x02);
    lcd_data(data[counter / 10]);
    lcd_data(data[counter % 10]);
  }

  return 0;
}
