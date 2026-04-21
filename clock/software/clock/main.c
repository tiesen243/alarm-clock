#include <io.h>
#include <stdio.h>

#include "sys/alt_irq.h"
#include "system.h"

#include "include/datetime.h"
#include "include/lcd.h"
#include "include/matrix-led.h"
#include "include/timer.h"
#include "include/uart.h"
#include "include/utils.h"

#define PER_TICK_MS 10

enum MODE { RUNNING, SET_TIME, SET_ALARM };
static enum MODE mode = RUNNING;

int timer_ticks = 0, alarm_ticks = 0;

#define BUFFER_SIZE 100
char buffer[BUFFER_SIZE];

struct DateTime current_time, alarm_time;
struct LCD lcd;
struct MatrixLed matrix_led;

void timer_irq_handler(void *context) {
  (void)context;

  timer_ticks += 1;
  if (timer_ticks == (1000 / PER_TICK_MS)) {
    timer_ticks = 0;
    DateTime_increment(&current_time);
  }

  if (mode == RUNNING)
    lcd_show_time(&lcd, &current_time);
  else if (mode == SET_TIME) {
    int is_done = set_datetime(&current_time, "Time seted", HEX_0_BASE,
                               HEX_1_BASE, BUTTON_BASE, SWITCH_BASE);
    if (is_done)
      mode = RUNNING;
  } else if (mode == SET_ALARM) {
    int is_done = set_datetime(&alarm_time, "Alarm seted", HEX_0_BASE,
                               HEX_1_BASE, BUTTON_BASE, SWITCH_BASE);
    if (is_done)
      mode = RUNNING;
  }

  if (alarm_ticks > 0 && mode == RUNNING) {
    alarm_ticks -= 1;
  }

  matrix_led_update_scroll(&matrix_led, "Hello, world! ");

  timer_cleanup(TIMER_0_BASE);
}

int main(void) {
  printf("Starting clock...\n");

  // Initialize the devices
  timer_init(TIMER_0_BASE, PER_TICK_MS);
  alt_ic_isr_register(0, TIMER_0_IRQ, timer_irq_handler, (void *)0, (void *)0);

  lcd_init(&lcd, LCD_ON_BASE, LCD_BLON_BASE, LCD_EN_BASE, LCD_RW_BASE,
           LCD_RS_BASE, LCD_DATA_BASE);

  matrix_led_init(&matrix_led, MATRIX_LED_DIN_BASE, MATRIX_LED_CS_BASE,
                  MATRIX_LED_CLK_BASE);

  // Initialize state
  DateTime_init(&current_time, 2026, 22, 4, 12, 0, 0);

  // Turn off the 7-segment displays
  IOWR(HEX_0_BASE, 0, 0xFF);
  IOWR(HEX_1_BASE, 0, 0xFF);

  printf("Clock started.\n");

  while (1) {
    // Button handlers
    if (IORD(BUTTON_BASE, 0) == 14) {
      while (IORD(BUTTON_BASE, 0) == 14)
        ;
      mode = RUNNING;
      alarm_ticks = 0;
      printf("Stop alarm via button\n");
      uart_send_string(UART_0_BASE, "A0");
      IOWR(BUZZER_BASE, 0, 0);
    } else if (IORD(BUTTON_BASE, 0) == 13 && mode == RUNNING) {
      while (IORD(BUTTON_BASE, 0) == 13)
        ;
      mode = SET_TIME;
      printf("Entering time setting mode\n");
      uart_send_string(UART_0_BASE, "Entering time setting mode");
    } else if (IORD(BUTTON_BASE, 0) == 11 && mode == RUNNING) {
      while (IORD(BUTTON_BASE, 0) == 11)
        ;
      mode = SET_ALARM;
      printf("Entering alarm setting mode\n");
      uart_send_string(UART_0_BASE, "Entering alarm setting mode");
    }

    // Check alarm
    if (is_alarm(&current_time, &alarm_time)) {
      if (alarm_ticks == 0) {
        printf("Alarm! Time is %02d:%02d:%02d\n", current_time.hour,
               current_time.minute, current_time.second);
        uart_send_string(UART_0_BASE, "A1");
        alarm_ticks = 300;
      }

      IOWR(BUZZER_BASE, 0, 1);
    }

    // UART command handlers
    if (is_uart_available(UART_0_BASE)) {
      uart_receive_string(UART_0_BASE, buffer, BUFFER_SIZE);
      printf("Received from UART: %s\n", buffer);

      if (buffer[0] == 'S' || buffer[0] == 's') {
        alarm_ticks = 0;
        IOWR(BUZZER_BASE, 0, 0);
        printf("Stop alarm via UART\n");
        uart_send_string(UART_0_BASE, "A0");
      } else if (buffer[0] == 'T' || buffer[0] == 't') {
        mode = SET_TIME;
        struct DateTime new_time;

        if (parse_uart_command(buffer, &new_time)) {
          current_time = new_time;
          printf("Time updated via UART\n");
          uart_send_string(UART_0_BASE, "Time updated");
        } else {
          printf("Invalid time format in UART command\n");
          uart_send_string(UART_0_BASE, "Invalid time format");
        }

        mode = RUNNING;
      } else if (buffer[0] == 'A' || buffer[0] == 'a') {
        mode = SET_ALARM;
        struct DateTime new_alarm;

        if (parse_uart_command(buffer, &new_alarm)) {
          alarm_time = new_alarm;
          printf("Alarm updated via UART\n");
          uart_send_string(UART_0_BASE, "Alarm updated");
        } else {
          printf("Invalid time format in UART command\n");
          uart_send_string(UART_0_BASE, "Invalid time format");
        }

        mode = RUNNING;
      }
    }
  }

  return 0;
}
