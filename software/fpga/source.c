/*
 * source.c
 *
 *  Created on: Mar 18, 2026
 *      Author: tiesen243
 */

#include "sys/alt_irq.h"
#include <ctype.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <stdint.h>

#include <system.h>
#include "include/datetime.h"
#include "include/lcd.h"
#include "include/timer_ctrl.h"
#include "include/uart.h"

enum MODE
{
  RUNNING,
  SET_TIME,
  SET_ALARM
};

static Date current_time = {25, 3, 2026, 18, 0, 0},
            alarm_time;
static enum MODE mode = RUNNING;
static int alarm_counter = 0, buzz_state = 0;
static char buffer[100];
int count = 0, count_2 = 0, i;

const uint8_t REG_DECODEMODE = 0x09;
const uint8_t REG_INTENSITY = 0x0A;
const uint8_t REG_SCANLIMIT = 0x0B;
const uint8_t REG_SHUTDOWN = 0x0C;

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

static void run_set_datetime(Date *target, const char *done_label);
static int parse_fixed_digits(const char *s, int len);
static int parse_uart_time_command(const char *cmd, Date *out);

void Timer_IQR_Handler(void *isr_context)
{
  (void)isr_context;

  count++;
  if (count >= 100)
  {
    count = 0;
    datetime_tick(&current_time);
  }

  if (mode == RUNNING)
    lcd_show_datetime(&current_time);
  else if (mode == SET_TIME)
    run_set_datetime(&current_time, "Time set");
  else if (mode == SET_ALARM)
    run_set_datetime(&alarm_time, "Alarm set");

  if (alarm_counter > 0 && mode == RUNNING)
  {
    count_2++;
    if (count_2 >= 10)
    {
      count_2 = 0;
      IOWR(BUZZ_BASE, 0, buzz_state);
      buzz_state = !buzz_state;
      alarm_counter--;
    }
  }
  else
  {
    IOWR(BUZZ_BASE, 0, 0);
  }

  timer_clear_timeout();
}

void send_matrix_led(uint8_t address, uint8_t data)
{
  IOWR(MATRIX_LED_CS_BASE, 0, 0);        // CS low
  IOWR(MATRIX_LED_DIN_BASE, 0, address); // Gửi địa chỉ
  IOWR(MATRIX_LED_CLK_BASE, 0, 1);       // Clock pulse
  IOWR(MATRIX_LED_CLK_BASE, 0, 0);
}

int main(void)
{
  lcd_init();
  lcd_power_on();
  lcd_show_datetime(&current_time);

  timer_init(10);
  alt_ic_isr_register(0, TIMER_IRQ, Timer_IQR_Handler, (void *)0, (void *)0);

  printf("Started!\n");

  // turn off led and 7-seg
  IOWR(HEX2_BASE, 0, 0xFF);
  IOWR(HEX1_BASE, 0, 0xFF);
  IOWR(HEX0_BASE, 0, 0xFF);

  uint8_t box[] = {
      0x00, // 00000000
      0x00, // 00000000
      0x3C, // 00111100
      0x3C, // 00111100
      0x3C, // 00111100
      0x3C, // 00111100
      0x00, // 00000000
      0x00  // 00000000
  };

  for (i = 0; i < 8; i++)
    send_matrix_led((uint8_t)(i + 1), box[i]);

  while (1)
  {
    if (IORD(BUTTON_BASE, 0) == 14)
    {
      while (IORD(BUTTON_BASE, 0) == 14)
        ;

      mode = RUNNING;
      alarm_counter = 0;
      printf("Alarm stopped via Button\n");
      uart_send_string("A0");
      IOWR(BUZZ_BASE, 0, 0);
      IOWR(LED_BASE, 0, 0);
    }
    else if (IORD(BUTTON_BASE, 0) == 13 && mode == RUNNING)
    {
      while (IORD(BUTTON_BASE, 0) == 13)
        ;

      mode = SET_TIME;
      printf("Mode: SET_TIME\n");
      uart_send_string("Set time mode activated");
    }
    else if (IORD(BUTTON_BASE, 0) == 11 && mode == RUNNING)
    {
      while (IORD(BUTTON_BASE, 0) == 11)
        ;

      mode = SET_ALARM;
      printf("Mode: SET_ALARM\n");
      uart_send_string("Set alarm mode activated");
    }

    if (is_alarm(&current_time, &alarm_time))
    {
      alarm_counter = 1000;
      uart_send_string("A1");
      IOWR(LED_BASE, 0, 1);
    }

    // UART handler
    if (is_uart_available())
    {
      uart_receive_string(buffer, sizeof(buffer));
      printf("Received from UART: %s\n", buffer);

      if (buffer[0] == 'S') // stop alarm command
      {
        alarm_counter = 0;
        IOWR(BUZZ_BASE, 0, 1);
        IOWR(LED_BASE, 0, 0);
        uart_send_string("A0");
        printf("Alarm stopped via UART.\n");
      }
      else if (buffer[0] == 'T') // set time command
      {
        mode = SET_TIME;
        Date new_time;

        if (parse_uart_time_command(buffer, &new_time))
        {
          current_time = new_time;
          printf("Time updated via UART: %02d/%02d/%04d %02d:%02d:%02d\n",
                 current_time.day, current_time.month, current_time.year,
                 current_time.hour, current_time.minute, current_time.second);
          {
            char uart_msg[100];
            snprintf(uart_msg, sizeof(uart_msg), "Time updated: %02d/%02d/%04d %02d:%02d:%02d",
                     current_time.day, current_time.month, current_time.year,
                     current_time.hour, current_time.minute, current_time.second);
            uart_send_string(uart_msg);
          }

          lcd_show_datetime(&current_time);
        }
        else
        {
          printf("Invalid time format received via UART.\n");
        }

        mode = RUNNING;
      }
      else if (buffer[0] == 'A') // set alarm command
      {
        mode = SET_ALARM;
        Date new_alarm;

        if (parse_uart_time_command(buffer, &new_alarm))
        {
          alarm_time = new_alarm;

          printf("Alarm updated via UART: %02d/%02d/%04d %02d:%02d:%02d\n",
                 alarm_time.day, alarm_time.month, alarm_time.year,
                 alarm_time.hour, alarm_time.minute, alarm_time.second);
          {
            char uart_msg[100];
            snprintf(uart_msg, sizeof(uart_msg), "Alarm updated: %02d/%02d/%04d %02d:%02d:%02d",
                     alarm_time.day, alarm_time.month, alarm_time.year,
                     alarm_time.hour, alarm_time.minute, alarm_time.second);
            uart_send_string(uart_msg);
          }

          alarm_counter = 0; // reset alarm counter to prevent immediate alarm if new time matches current time
        }
        else
        {
          printf("Invalid alarm format received via UART.\n");
        }

        mode = RUNNING;
      }
    }
  }

  return 0;
}

int state = 0;

static int normalize_switch_value(int switch_data, int field_state)
{
  if (field_state == 0) // year
    switch_data = switch_data + 2000;
  else if (field_state == 1) // month
    switch_data = switch_data % 13;
  else if (field_state == 2) // day
  {
    int is_leap_year = ((current_time.year % 4 == 0 && current_time.year % 100 != 0) || (current_time.year % 400 == 0));
    int is_month_31_days = (current_time.month == 1 || current_time.month == 3 || current_time.month == 5 || current_time.month == 7 || current_time.month == 8 || current_time.month == 10 || current_time.month == 12);

    if (is_month_31_days)
      switch_data = switch_data % 32;
    else if (current_time.month == 2)
      switch_data = switch_data % (is_leap_year ? 30 : 29);
    else
      switch_data = switch_data % 31;
  }
  else if (field_state == 3)
    switch_data = switch_data % 24;
  else if (field_state == 4)
    switch_data = switch_data % 60;
  else if (field_state == 5)
    switch_data = switch_data % 60;

  return switch_data;
}

static void apply_datetime_field(Date *target, int field_state, int switch_data)
{
  switch (field_state)
  {
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

static void run_set_datetime(Date *target, const char *done_label)
{
  int switch_data = IORD(SWITCH_BASE, 0);

  if (switch_data == 0)
    return;

  switch_data = normalize_switch_value(switch_data, state);

  IOWR(HEX2_BASE, 0, 0xFF);
  IOWR(HEX1_BASE, 0, HEX_7SEG[(switch_data / 10) % 10]);
  IOWR(HEX0_BASE, 0, HEX_7SEG[switch_data % 10]);

  if (IORD(BUTTON_BASE, 0) == 7)
  {
    while (IORD(BUTTON_BASE, 0) == 7)
      ;

    switch (state)
    {
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
      printf("%s: %02d/%02d/%04d %02d:%02d:%02d\n", done_label,
             target->day, target->month, target->year,
             target->hour, target->minute, target->second);
      mode = RUNNING;
      alarm_counter = 0;
      state = -1;

      IOWR(HEX2_BASE, 0, 0xFF);
      IOWR(HEX1_BASE, 0, 0xFF);
      IOWR(HEX0_BASE, 0, 0xFF);
      break;
    }

    lcd_show_datetime(target);
    state += 1;
  }
}

static int parse_fixed_digits(const char *s, int len)
{
  int value = 0;
  int i;

  for (i = 0; i < len; i++)
  {
    if (!isdigit((unsigned char)s[i]))
      return -1;

    value = value * 10 + (s[i] - '0');
  }

  return value;
}

static int parse_uart_time_command(const char *cmd, Date *out)
{
  int day, month, year, hour, minute, second;

  day = parse_fixed_digits(cmd + 1, 2);
  month = parse_fixed_digits(cmd + 3, 2);
  year = parse_fixed_digits(cmd + 5, 4);
  hour = parse_fixed_digits(cmd + 9, 2);
  minute = parse_fixed_digits(cmd + 11, 2);
  second = parse_fixed_digits(cmd + 13, 2);

  printf("Parsed UART command - Day: %d, Month: %d, Year: %d, Hour: %d, Minute: %d, Second: %d\n",
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
