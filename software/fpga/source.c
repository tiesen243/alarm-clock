/*
 * source.c
 *
 *  Created on: Mar 18, 2026
 *      Author: tiesen243
 */

#include "sys/alt_irq.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "io.h"
#include "system.h"

#include "include/datetime.h"
#include "include/lcd.h"
#include "include/matrix_led.h"
#include "include/timer_ctrl.h"
#include "include/uart.h"
#include "include/utils.h"

enum MODE { RUNNING, SET_TIME, SET_ALARM };

static Date current_time = {25, 3, 2026, 18, 0, 0}, alarm_time;
static enum MODE mode = RUNNING;
static int alarm_counter = 0, buzz_state = 0;
int tick_divider = 0, buzzer_timer = 0, revert_divider = 0, i;
static char buffer[100];
static is_show_time = 0;
int led = 0x00001;

void Timer_IQR_Handler(void *isr_context) {
  (void)isr_context;

  tick_divider++;
  if (tick_divider >= 100) {
    tick_divider = 0;
    datetime_tick(&current_time);
  }

  revert_divider++;
  if (revert_divider >= 500) {
    revert_divider = 0;
    is_show_time = !is_show_time;
  }

  if (mode == RUNNING) {
    lcd_show_datetime(&current_time);
    hex_show_time(&current_time, is_show_time);
  } else if (mode == SET_TIME) {
    int is_done = run_set_datetime(&current_time, "Time set");
    if (is_done) {
      mode = RUNNING;
      alarm_counter = 0;
      uart_send_string("Time set mode completed");
    }
  } else if (mode == SET_ALARM) {
    int is_done = run_set_datetime(&alarm_time, "Alarm set");
    if (is_done) {
      mode = RUNNING;
      alarm_counter = 0;
      uart_send_string("Alarm set mode completed");
    }
  }

  if (alarm_counter > 0 && mode == RUNNING) {
    buzzer_timer++;
    if (buzzer_timer >= 10) {
      buzzer_timer = 0;
      IOWR(BUZZ_BASE, 0, buzz_state);
      buzz_state = !buzz_state;
      alarm_counter--;

      led <<= 1;
      if (led > 0x20000)
        led = 0x0001;
      IOWR(LED_BASE, 0, led);
    }

    update_matrix_text_scroll();
  } else {
    IOWR(BUZZ_BASE, 0, 1);
    clear_matrix_led();
    IOWR(LED_BASE, 0, 0x00);
  }

  timer_clear_timeout();
}

int main(void) {
  lcd_init();
  lcd_power_on();
  lcd_show_datetime(&current_time);

  timer_init(10);
  alt_ic_isr_register(0, TIMER_IRQ, Timer_IQR_Handler, (void *)0, (void *)0);

  matrix_led_init();

  printf("Started!\n");

  // turn off led and 7-seg
  IOWR(HEX2_BASE, 0, 0xFF);
  IOWR(HEX1_BASE, 0, 0xFF);
  IOWR(HEX0_BASE, 0, 0xFF);
  IOWR(BUZZ_BASE, 0, 0);

  while (1) {
    if (IORD(BUTTON_BASE, 0) == 14 && mode == RUNNING) {
      while (IORD(BUTTON_BASE, 0) == 14)
        ;

      alarm_counter = 0;
      printf("Alarm stopped via Button\n");
      uart_send_string("A0");
      IOWR(BUZZ_BASE, 0, 0);
    } else if (IORD(BUTTON_BASE, 0) == 13 && mode == RUNNING) {
      while (IORD(BUTTON_BASE, 0) == 13)
        ;

      mode = SET_TIME;
      printf("Mode: SET_TIME\n");
      uart_send_string("Set time mode activated");
    } else if (IORD(BUTTON_BASE, 0) == 11 && mode == RUNNING) {
      while (IORD(BUTTON_BASE, 0) == 11)
        ;

      mode = SET_ALARM;
      printf("Mode: SET_ALARM\n");
      uart_send_string("Set alarm mode activated");
    }

    if (is_alarm(&current_time, &alarm_time)) {
      alarm_counter = 100;
      uart_send_string("A1");
      IOWR(LED_BASE, 0, 1);
    }

    // UART handler
    if (is_uart_available()) {
      uart_receive_string(buffer, sizeof(buffer));
      printf("Received from UART: %s\n", buffer);

      if (buffer[0] == 'S') // stop alarm command
      {
        alarm_counter = 0;
        IOWR(LED_BASE, 0, 0);
        uart_send_string("A0");
        printf("Alarm stopped via UART.\n");
      } else if (buffer[0] == 'T') // set time command
      {
        mode = SET_TIME;
        Date new_time;

        if (parse_uart_time_command(buffer, &new_time)) {
          current_time = new_time;

          char uart_msg[100];
          snprintf(uart_msg, sizeof(uart_msg),
                   "Time updated: %02d/%02d/%04d %02d:%02d:%02d",
                   current_time.day, current_time.month, current_time.year,
                   current_time.hour, current_time.minute, current_time.second);
          uart_send_string(uart_msg);
          printf(uart_msg);
        } else
          printf("Invalid time format received via UART.\n");

        mode = RUNNING;
      } else if (buffer[0] == 'A') // set alarm command
      {
        mode = SET_ALARM;
        Date new_alarm;

        if (parse_uart_time_command(buffer, &new_alarm)) {
          alarm_time = new_alarm;

          char uart_msg[100];
          snprintf(uart_msg, sizeof(uart_msg),
                   "Alarm updated: %02d/%02d/%04d %02d:%02d:%02d",
                   alarm_time.day, alarm_time.month, alarm_time.year,
                   alarm_time.hour, alarm_time.minute, alarm_time.second);
          uart_send_string(uart_msg);
          printf(uart_msg);

          alarm_counter = 0;
        } else
          printf("Invalid alarm format received via UART.\n");

        mode = RUNNING;
      }
    }
  }

  return 0;
}
