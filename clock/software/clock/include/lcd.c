#include "lcd.h"
#include "datetime.h"

void lcd_delay(int ms) {
  volatile int i = 0;
  while (i < ms * 10000)
    i++;
}

void lcd_command(struct LCD *lcd, int data) {
  IOWR_ALTERA_AVALON_PIO_DATA(lcd->rs, 0x00);
  IOWR_ALTERA_AVALON_PIO_DATA(lcd->rw, 0x00);
  IOWR_ALTERA_AVALON_PIO_DATA(lcd->data, data & 0xFF);
  IOWR_ALTERA_AVALON_PIO_DATA(lcd->en, 0x01);
  lcd_delay(20);

  IOWR_ALTERA_AVALON_PIO_DATA(lcd->en, 0x00);
  lcd_delay(20);
}

void lcd_data(struct LCD *lcd, char data) {
  IOWR_ALTERA_AVALON_PIO_DATA(lcd->rs, 0x01);
  IOWR_ALTERA_AVALON_PIO_DATA(lcd->rs, 0x00);
  IOWR_ALTERA_AVALON_PIO_DATA(lcd->data, data & 0xFF);
  IOWR_ALTERA_AVALON_PIO_DATA(lcd->en, 0x01);
  lcd_delay(20);

  IOWR_ALTERA_AVALON_PIO_DATA(lcd->en, 0x00);
  lcd_delay(20);
}

void lcd_string(struct LCD *lcd, const char *str) {
  volatile int i = 0;

  while (str[i] != 0) {
    lcd_data(lcd, str[i]);
    i++;
  }
}

void lcd_init(struct LCD *lcd, int on_pin, int blon_pin, int en_pin, int rw_pin,
              int rs_pin, int data_pin) {
  lcd->on = on_pin;
  lcd->blon = blon_pin;
  lcd->en = en_pin;
  lcd->rw = rw_pin;
  lcd->rs = rs_pin;
  lcd->data = data_pin;

  lcd_command(lcd, 0x38);
  lcd_delay(100);
  lcd_command(lcd, 0x0C);
  lcd_delay(100);
  lcd_command(lcd, 0x06);
  lcd_delay(100);
  lcd_command(lcd, 0x01);
  lcd_delay(100);

  IOWR_ALTERA_AVALON_PIO_DATA(lcd->on, 0x01);
  IOWR_ALTERA_AVALON_PIO_DATA(lcd->blon, 0x01);
}

static struct DateTime last_time;
int has_last_time = 0;

void lcd_2digit(struct LCD *lcd, int value) {
  lcd_data(lcd, '0' + (value / 10) % 10);
  lcd_data(lcd, '0' + value % 10);
}

void lcd_4digit(struct LCD *lcd, int value) {
  lcd_data(lcd, '0' + (value / 1000) % 10);
  lcd_data(lcd, '0' + (value / 100) % 10);
  lcd_data(lcd, '0' + (value / 10) % 10);
  lcd_data(lcd, '0' + value % 10);
}

void lcd_show_time(struct LCD *lcd, struct DateTime *dt) {
  if (!dt)
    return;

  if (!has_last_time) {
    lcd_command(lcd, 0x02);
    lcd_2digit(lcd, dt->day);
    lcd_data(lcd, '/');
    lcd_2digit(lcd, dt->month);
    lcd_data(lcd, '/');
    lcd_4digit(lcd, dt->year);

    lcd_command(lcd, 0xC0);
    lcd_2digit(lcd, dt->hour);
    lcd_data(lcd, ':');
    lcd_2digit(lcd, dt->minute);
    lcd_data(lcd, ':');
    lcd_2digit(lcd, dt->second);

    has_last_time = 1;
    last_time = *dt;
    return;
  }

  if (dt->day != last_time.day) {
    lcd_command(lcd, 0x02);
    lcd_2digit(lcd, dt->day);
  }

  if (dt->month != last_time.month) {
    lcd_command(lcd, 0x05);
    lcd_2digit(lcd, dt->month);
  }

  if (dt->year != last_time.year) {
    lcd_command(lcd, 0x08);
    lcd_4digit(lcd, dt->year);
  }

  if (dt->hour != last_time.hour) {
    lcd_command(lcd, 0xC0);
    lcd_2digit(lcd, dt->hour);
  }

  if (dt->minute != last_time.minute) {
    lcd_command(lcd, 0xC3);
    lcd_2digit(lcd, dt->minute);
  }

  if (dt->second != last_time.second) {
    lcd_command(lcd, 0xC6);
    lcd_2digit(lcd, dt->second);
  }

  last_time = *dt;
}
