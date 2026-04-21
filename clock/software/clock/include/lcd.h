#ifndef LCD_H
#define LCD_H

#include "altera_avalon_pio_regs.h"

#include "datetime.h"

struct LCD {
  int on;
  int blon;
  int en;
  int rw;
  int rs;
  int data;
};

void lcd_delay(int ms);

void lcd_command(struct LCD *lcd, int data);

void lcd_data(struct LCD *lcd, char data);

void lcd_string(struct LCD *lcd, const char *str);

void lcd_init(struct LCD *lcd, int on_pin, int blon_pin, int en_pin, int rw_pin,
              int rs_pin, int data_pin);

void lcd_show_time(struct LCD *lcd, struct DateTime *dt);

#endif // !LCD_H
