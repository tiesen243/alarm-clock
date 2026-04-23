#include <stdio.h>
#include <string.h>

#include "io.h"

#include "datetime.h"
#include "system.h"
#include "utils.h"

const int HEX_7SEG[16] = {
    0x40, // 0
    0x79, // 1
    0x24, // 2
    0x30, // 3
    0x19, // 4
    0x12, // 5
    0x02, // 6
    0x78, // 7
    0x00, // 8
    0x10, // 9
    0x08, // A
    0x03, // b
    0x46, // C
    0x21, // d
    0x06, // E
    0x0E  // F
};

static int normalize_switch_value(struct DateTime *current_time,
                                  int switch_data, int field_state) {
  if (field_state == 0) // year
    switch_data = switch_data + 2000;
  else if (field_state == 1) // month
    switch_data = switch_data % 13;
  else if (field_state == 2) // day
  {
    int is_leap_year =
        ((current_time->year % 4 == 0 && current_time->year % 100 != 0) ||
         (current_time->year % 400 == 0));
    int is_month_31_days =
        (current_time->month == 1 || current_time->month == 3 ||
         current_time->month == 5 || current_time->month == 7 ||
         current_time->month == 8 || current_time->month == 10 ||
         current_time->month == 12);

    if (is_month_31_days)
      switch_data = switch_data % 32;
    else if (current_time->month == 2)
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

static int field_state = 0;

int set_datetime(struct DateTime *target, const char *label) {
  int switch_data = IORD(SWITCH_BASE, 0);

  if (switch_data == 0)
    return 0;

  switch_data = normalize_switch_value(target, switch_data, field_state);

  IOWR(HEX_1_BASE, 0, HEX_7SEG[(switch_data / 10) % 10]);
  IOWR(HEX_0_BASE, 0, HEX_7SEG[switch_data % 10]);

  if (IORD(BUTTON_BASE, 0) == 7) {
    while (IORD(BUTTON_BASE, 0) == 7)
      ;

    switch (field_state) {
    case 0:
      printf("%s: year to %d\n", label, switch_data);
      target->year = switch_data;
      break;
    case 1:
      printf("%s: month to %d\n", label, switch_data);
      target->month = switch_data;
      break;
    case 2:
      printf("%s: day to %d\n", label, switch_data);
      target->day = switch_data;
      break;
    case 3:
      printf("%s: hour to %d\n", label, switch_data);
      target->hour = switch_data;
      break;
    case 4:
      printf("%s: minute to %d\n", label, switch_data);
      target->minute = switch_data;
      break;
    case 5:
      printf("%s: second to %d\n", label, switch_data);
      target->second = switch_data;
      break;
    case 6:
      printf("%s: %02d/%02d/%04d %02d:%02d:%02d\n", label, target->day,
             target->month, target->year, target->hour, target->minute,
             target->second);

      IOWR(HEX_0_BASE, 0, 0xFF);
      IOWR(HEX_1_BASE, 0, 0xFF);
      break;
    }
  }

  field_state = (field_state + 1) % 7;
  return field_state == 0;
}

int is_alarm(struct DateTime *current, struct DateTime *alarm) {
  return current->hour == alarm->hour && current->minute == alarm->minute &&
         current->second == alarm->second;
}

int is_leap_year(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int days_in_month(int month, int year) {
  switch (month) {
  case 1:
  case 3:
  case 5:
  case 7:
  case 8:
  case 10:
  case 12:
    return 31;
  case 4:
  case 6:
  case 9:
  case 11:
    return 30;
  case 2:
    return is_leap_year(year) ? 29 : 28;
  default:
    return 0;
  }
}

int parse_uart_command(const char *cmd, struct DateTime *dt) {
  if (strlen(cmd) != 16 || (cmd[0] != 'T' && cmd[0] != 't') ||
      (cmd[1] != 'A' && cmd[1] != 'a'))
    return 0;

  dt->year = (cmd[2] - '0') * 1000 + (cmd[3] - '0') * 100 +
             (cmd[4] - '0') * 10 + (cmd[5] - '0');
  if (dt->year < 0 || dt->year > 9999)
    return 0;

  dt->month = (cmd[6] - '0') * 10 + (cmd[7] - '0');
  if (dt->month < 1 || dt->month > 12)
    return 0;

  dt->day = (cmd[8] - '0') * 10 + (cmd[9] - '0');
  if (dt->day < 1 || dt->day > days_in_month(dt->month, dt->year))
    return 0;

  dt->hour = (cmd[10] - '0') * 10 + (cmd[11] - '0');
  if (dt->hour < 0 || dt->hour > 23)
    return 0;

  dt->minute = (cmd[12] - '0') * 10 + (cmd[13] - '0');
  if (dt->minute < 0 || dt->minute > 59)
    return 0;

  dt->second = (cmd[14] - '0') * 10 + (cmd[15] - '0');
  if (dt->second < 0 || dt->second > 59)
    return 0;

  printf("Parsed UART command: %02d/%02d/%04d %02d:%02d:%02d\n", dt->day,
         dt->month, dt->year, dt->hour, dt->minute, dt->second);

  return 1;
}
