#include "io.h"

#include "matrix-led.h"

int i = 0, row = 0;
const char *p;
const int FONT[256][6] = {
    ['H'] = {4, 0xFF, 0x18, 0x18, 0xFF},
    ['e'] = {4, 0x78, 0xA4, 0xA4, 0x58},
    ['l'] = {3, 0x82, 0xFF, 0x80},
    ['o'] = {4, 0x7C, 0x82, 0x82, 0x7C},
    ['W'] = {5, 0x7F, 0x80, 0x70, 0x80, 0x7F},
    ['r'] = {4, 0xFC, 0x10, 0x08, 0x08},
    ['d'] = {4, 0x78, 0x84, 0x84, 0xFF},
    ['!'] = {1, 0xBF},
    [' '] = {3, 0x00, 0x00, 0x00},
};

void matrix_led_init(struct MatrixLed *led, int din_pin, int cs_pin,
                     int clk_pin) {
  led->din_pin = din_pin;
  led->cs_pin = cs_pin;
  led->clk_pin = clk_pin;

  IOWR(led->clk_pin, 0, 0);
  IOWR(led->cs_pin, 0, 1);

  matrix_led_write_reg(led, REG_DISPLAY_TEST, 0x00);
  matrix_led_write_reg(led, REG_SHUTDOWN, 0x01);
  matrix_led_write_reg(led, REG_DECODE_MODE, 0x00);
  matrix_led_write_reg(led, REG_SCAN_LIMIT, 0x07);
  matrix_led_write_reg(led, REG_INTENSITY, 0x02);

  for (i = 1; i <= 8; i++)
    matrix_led_write_reg(led, i, 0x00);
}

void matrix_led_shift_out(struct MatrixLed *led, int byte) {
  for (i = 0; i < 8; i++) {
    int bit = (byte >> (7 - i)) & 0x01;
    IOWR(led->din_pin, 0, bit);
    IOWR(led->clk_pin, 0, 1);
    IOWR(led->clk_pin, 0, 0);
  }
}

void matrix_led_write_reg(struct MatrixLed *led, int reg, int data) {
  IOWR(led->cs_pin, 0, 0);
  matrix_led_shift_out(led, reg);
  matrix_led_shift_out(led, data);
  IOWR(led->cs_pin, 0, 1);
}

int build_bitmap(const char *str, int *bitmap) {
  int idx = 0;

  for (p = str; *p != '\0'; p++) {
    unsigned char c = (unsigned char)*p;

    if (FONT[c][0] == 0)
      continue;

    int width = FONT[c][0];
    for (i = 1; i <= width; i++)
      bitmap[idx++] = FONT[c][i];

    bitmap[idx++] = 0x00;
  }

  return idx;
}

static int display_buffer[] = {0, 0, 0, 0, 0, 0, 0, 0}, scroll_idx = 0;

void matrix_led_update_scroll(struct MatrixLed *led, const char *str) {
  int full_bitmap[256];
  int bitmap_length = build_bitmap(str, full_bitmap);

  int new_column = full_bitmap[scroll_idx];

  for (row = 0; row < 8; row++) {
    int new_bit = (new_column >> row) & 0x01;
    display_buffer[row] = ((display_buffer[row] << 1) | new_bit) & 0xFF;
  }

  for (i = 0; i < 8; i++)
    matrix_led_write_reg(led, REG_DIGIT0 + i, display_buffer[i]);

  scroll_idx = (scroll_idx + 1) % bitmap_length;
}
