#ifndef MATRIX_LED_H

struct MatrixLed {
  int din_pin;
  int cs_pin;
  int clk_pin;
};

#define REG_DIGIT0 0x01
#define REG_SHUTDOWN 0x0C
#define REG_SCAN_LIMIT 0x0B
#define REG_INTENSITY 0x0A
#define REG_DECODE_MODE 0x09
#define REG_DISPLAY_TEST 0x0F

void matrix_led_init(struct MatrixLed *led, int din_pin, int cs_pin,
                     int clk_pin);

void matrix_led_shift_out(struct MatrixLed *led, int byte);

void matrix_led_write_reg(struct MatrixLed *led, int reg, int data);

void matrix_led_update_scroll(struct MatrixLed *led, const char *str);

#endif // !MATRIX_LED_H
