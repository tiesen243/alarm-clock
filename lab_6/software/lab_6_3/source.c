/*
 * source.c
 *
 *  Created on: Feb 1, 2026
 *  Author: tiesen243
 */

#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include "sys/alt_stdio.h"
#include "system.h"
#include <stdio.h>
#include <time.h>

unsigned int counter = 1769935447;

void delay(int a) {
  volatile int i = 0;
  // while (i < a * 10000)
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

char *calculate_date(unsigned int current) {
  static char date_str[11];
  time_t seconds = (time_t)current;

  struct tm *t = localtime(&seconds);

  sprintf(date_str, "%02d-%02d-%04d", t->tm_mday, t->tm_mon + 1,
          t->tm_year + 1900);

  return date_str;
}

char *calculate_time(unsigned int current) {
  static char time_str[9];
  time_t seconds = (time_t)current;

  struct tm *t = localtime(&seconds);

  sprintf(time_str, "%02d:%02d:%02d", t->tm_hour + 7, t->tm_min, t->tm_sec);

  return time_str;
}

void Timer_IQR_Handler(void *isr_context) {
  counter++;

  command(0x02);

  char *time_str = calculate_time(counter);
  lcd_string(time_str);

  command(0xC0); // Move to second line

  char *date_str = calculate_date(counter);
  lcd_string(date_str);

  IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE,
                                  ALTERA_AVALON_TIMER_STATUS_TO_MSK);
}

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

  lcd_init();
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_ON_BASE, 0x01);
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_BLON_BASE, 0x00);

  while (1)
    ;

  return 0;
}
