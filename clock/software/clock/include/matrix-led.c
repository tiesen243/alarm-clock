#include "io.h"
#include "system.h"

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

void matrix_led_init() {

  IOWR(MATRIX_LED_CLK_BASE, 0, 0);
  IOWR(MATRIX_LED_CLK_BASE, 0, 1);

  matrix_led_write_reg(REG_DISPLAY_TEST, 0x00);
  matrix_led_write_reg(REG_SHUTDOWN, 0x01);
  matrix_led_write_reg(REG_DECODE_MODE, 0x00);
  matrix_led_write_reg(REG_SCAN_LIMIT, 0x07);
  matrix_led_write_reg(REG_INTENSITY, 0x02);

  for (i = 1; i <= 8; i++)
    matrix_led_write_reg(i, 0x00);
}

void matrix_led_shift_out(int byte) {
  for (i = 0; i < 8; i++) {
    int bit = (byte >> (7 - i)) & 0x01;
    IOWR(MATRIX_LED_DIN_BASE, 0, bit);
    IOWR(MATRIX_LED_CLK_BASE, 0, 1);
    IOWR(MATRIX_LED_CLK_BASE, 0, 0);
  }
}

void matrix_led_write_reg(int reg, int data) {
  IOWR(MATRIX_LED_CS_BASE, 0, 0);
  matrix_led_shift_out(reg);
  matrix_led_shift_out(data);
  IOWR(MATRIX_LED_CS_BASE, 0, 1);
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

void matrix_led_update_scroll(const char *str) {
  int full_bitmap[256];
  int bitmap_length = build_bitmap(str, full_bitmap);

  int new_column = full_bitmap[scroll_idx];

  for (row = 0; row < 8; row++) {
    int new_bit = (new_column >> row) & 0x01;
    display_buffer[row] = ((display_buffer[row] << 1) | new_bit) & 0xFF;
  }

  for (i = 0; i < 8; i++)
    matrix_led_write_reg(REG_DIGIT0 + i, display_buffer[i]);

  scroll_idx = (scroll_idx + 1) % bitmap_length;
}
