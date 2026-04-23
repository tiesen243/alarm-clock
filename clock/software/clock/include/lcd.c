#include "lcd.h"
#include "datetime.h"
#include "system.h"

void lcd_delay(int ms) {
  volatile int i = 0;
  while (i < ms * 1000)
    i++;
}

void lcd_command(int data) {
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_RS_BASE, 0x00);
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_RW_BASE, 0x00);
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_DATA_BASE, data & 0xFF);
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_EN_BASE, 0x01);
  lcd_delay(5);

  IOWR_ALTERA_AVALON_PIO_DATA(LCD_EN_BASE, 0x00);
  lcd_delay(5);
}

void lcd_data(char data) {
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_RS_BASE, 0x01);
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_RS_BASE, 0x00);
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_DATA_BASE, data & 0xFF);
  IOWR_ALTERA_AVALON_PIO_DATA(LCD_EN_BASE, 0x01);
  lcd_delay(5);

  IOWR_ALTERA_AVALON_PIO_DATA(LCD_EN_BASE, 0x00);
  lcd_delay(5);
}

void lcd_string(const char *str) {
  volatile int i = 0;

  while (str[i] != 0) {
    lcd_data(str[i]);
    i++;
  }
}

void lcd_init(void)
{
    lcd_command(0x38);
    lcd_delay(100);
    lcd_command(0x0c);
    lcd_delay(100);
    lcd_command(0x06);
    lcd_delay(100);
    lcd_command(0x01);


    IOWR_ALTERA_AVALON_PIO_DATA(LCD_ON_BASE, 0x01);
    IOWR_ALTERA_AVALON_PIO_DATA(LCD_BLON_BASE, 0x01);
}
static struct DateTime lastDate;
int hasLastDate = 0;

static void lcd_set_cursor(int row, int col)
{
    int address = (row == 0 ? 0x00 : 0x40) + col;
    lcd_command(0x80 | address);
}

static void lcd_write_2digits(int value)
{
    lcd_data(((value / 10) % 10) + '0');
    lcd_data((value % 10) + '0');
}

static void lcd_write_4digits(int value)
{
    lcd_data((value / 1000) + '0');
    lcd_data(((value / 100) % 10) + '0');
    lcd_data(((value / 10) % 10) + '0');
    lcd_data((value % 10) + '0');
}

void lcd_show_time(struct DateTime *date)
{
    if (date == 0)
        return;

    if (!hasLastDate)
    {
        lcd_command(0x02);
        lcd_write_2digits(date->day);
        lcd_data('/');
        lcd_write_2digits(date->month);
        lcd_data('/');
        lcd_write_4digits(date->year);

        lcd_command(0xc0);
        lcd_write_2digits(date->hour);
        lcd_data(':');
        lcd_write_2digits(date->minute);
        lcd_data(':');
        lcd_write_2digits(date->second);

        lastDate = *date;
        hasLastDate = 1;
        return;
    }

    if (date->day != lastDate.day)
    {
        lcd_set_cursor(0, 0);
        lcd_write_2digits(date->day);
    }

    if (date->month != lastDate.month)
    {
        lcd_set_cursor(0, 3);
        lcd_write_2digits(date->month);
    }

    if (date->year != lastDate.year)
    {
        lcd_set_cursor(0, 6);
        lcd_write_4digits(date->year);
    }

    if (date->hour != lastDate.hour)
    {
        lcd_set_cursor(1, 0);
        lcd_write_2digits(date->hour);
    }

    if (date->minute != lastDate.minute)
    {
        lcd_set_cursor(1, 3);
        lcd_write_2digits(date->minute);
    }

    if (date->second != lastDate.second)
    {
        lcd_set_cursor(1, 6);
        lcd_write_2digits(date->second);
    }

    lastDate = *date;
}
