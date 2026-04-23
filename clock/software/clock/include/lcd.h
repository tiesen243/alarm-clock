#ifndef LCD_H
#define LCD_H

#include "altera_avalon_pio_regs.h"

#include "datetime.h"

void lcd_delay(int ms);

void lcd_command(int data);

void lcd_data(char data);

void lcd_string(const char *str);

void lcd_init();

void lcd_show_time(struct DateTime *dt);

#endif // !LCD_H
