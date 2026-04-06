#ifndef LCD_H
#define LCD_H

#include "datetime.h"

void lcd_init(void);
void lcd_power_on(void);
void lcd_show_datetime(const Date *date);

#endif // LCD_H
