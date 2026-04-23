#ifndef MATRIX_LED_H
#define MATRIX_LED_H

#include <stdint.h>

void send_matrix_led(uint8_t address, uint8_t data);

void matrix_led_init(void);

void update_matrix_text_scroll();

void clear_matrix_led();

#endif // !#ifndef MATRIX_LED_H
