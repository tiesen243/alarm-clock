#include "utils.h"

#include <ctype.h>
#include <stdio.h>

#include "io.h"
#include "system.h"

#include "datetime.h"
#include "lcd.h"
#include "uart.h"

int state = 0;

static int normalize_switch_value(Date current_time, int switch_data,
                                  int field_state) {
  if (field_state == 0) // year
    switch_data = switch_data + 2000;
  else if (field_state == 1) // month
    switch_data = switch_data % 13;
  else if (field_state == 2) // day
  {
    int is_leap_year =
        ((current_time.year % 4 == 0 && current_time.year % 100 != 0) ||
         (current_time.year % 400 == 0));
    int is_month_31_days =
        (current_time.month == 1 || current_time.month == 3 ||
         current_time.month == 5 || current_time.month == 7 ||
         current_time.month == 8 || current_time.month == 10 ||
         current_time.month == 12);

    if (is_month_31_days)
      switch_data = switch_data % 32;
    else if (current_time.month == 2)
      switch_data = switch_data % (is_leap_year ? 30 : 29);
    else
      switch_data = switch_data % 31;
  } else if (field_state == 3)
    switch_data = switch_data % 24;
  else if (field_state == 4)
    switch_data = switch_data % 60;
  else if (field_state == 5)
    switch_data = switch_data % 60;

  return switch_data;
}

static void apply_datetime_field(Date *target, int field_state,
                                 int switch_data) {
  switch (field_state) {
  case 0:
    target->year = switch_data;
    break;
  case 1:
    target->month = switch_data;
    break;
  case 2:
    target->day = switch_data;
    break;
  case 3:
    target->hour = switch_data;
    break;
  case 4:
    target->minute = switch_data;
    break;
  case 5:
    target->second = switch_data;
    break;
  default:
    break;
  }
}

int run_set_datetime(Date *target, const char *done_label) {
  int switch_data = IORD(SWITCH_BASE, 0);

  if (switch_data == 0)
    return 0;

  switch_data = normalize_switch_value(*target, switch_data, state);

  IOWR(HEX2_BASE, 0, 0xFF);
  IOWR(HEX1_BASE, 0, HEX_7SEG[(switch_data / 10) % 10]);
  IOWR(HEX0_BASE, 0, HEX_7SEG[switch_data % 10]);

  if (IORD(BUTTON_BASE, 0) == 7) {
    while (IORD(BUTTON_BASE, 0) == 7)
      ;

    switch (state) {
    case 0:
      printf("Set year: %d\n", (switch_data));
      apply_datetime_field(target, state, switch_data);
      uart_send_string("Year set");
      break;
    case 1:
      printf("Set month: %d\n", switch_data);
      apply_datetime_field(target, state, switch_data);
      uart_send_string("Month set");
      break;
    case 2:
      printf("Set day: %d\n", switch_data);
      apply_datetime_field(target, state, switch_data);
      uart_send_string("Day set");
      break;
    case 3:
      printf("Set hour: %d\n", switch_data);
      apply_datetime_field(target, state, switch_data);
      uart_send_string("Hour set");
      break;
    case 4:
      printf("Set minute: %d\n", switch_data);
      apply_datetime_field(target, state, switch_data);
      uart_send_string("Minute set");
      break;
    case 5:
      printf("Set second: %d\n", switch_data);
      apply_datetime_field(target, state, switch_data);
      uart_send_string("Second set");
      break;
    case 6:
      printf("%s: %02d/%02d/%04d %02d:%02d:%02d\n", done_label, target->day,
             target->month, target->year, target->hour, target->minute,
             target->second);

      IOWR(HEX2_BASE, 0, 0xFF);
      IOWR(HEX1_BASE, 0, 0xFF);
      IOWR(HEX0_BASE, 0, 0xFF);
      break;
    }

    state = (state + 1) & 7;
    lcd_show_datetime(target);
    return (state > 6);
  }

  return 0;
}

int parse_fixed_digits(const char *s, int len) {
  int value = 0;
  int i;

  for (i = 0; i < len; i++) {
    if (!isdigit((unsigned char)s[i]))
      return -1;

    value = value * 10 + (s[i] - '0');
  }

  return value;
}

int parse_uart_time_command(const char *cmd, Date *out) {
  int day, month, year, hour, minute, second;

  day = parse_fixed_digits(cmd + 1, 2);
  month = parse_fixed_digits(cmd + 3, 2);
  year = parse_fixed_digits(cmd + 5, 4);
  hour = parse_fixed_digits(cmd + 9, 2);
  minute = parse_fixed_digits(cmd + 11, 2);
  second = parse_fixed_digits(cmd + 13, 2);

  printf("Parsed UART command - Day: %d, Month: %d, Year: %d, Hour: %d, "
         "Minute: %d, Second: %d\n",
         day, month, year, hour, minute, second);

  if (day < 1 || day > 31)
    return 0;
  if (month < 1 || month > 12)
    return 0;
  if (year < 0 || year > 9999)
    return 0;
  if (hour < 0 || hour > 23)
    return 0;
  if (minute < 0 || minute > 59)
    return 0;
  if (second < 0 || second > 59)
    return 0;

  out->day = day;
  out->month = month;
  out->year = year;
  out->hour = hour;
  out->minute = minute;
  out->second = second;

  return 1;
}
