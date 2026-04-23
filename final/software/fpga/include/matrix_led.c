#include "matrix_led.h"

#include <string.h>

#include "io.h"
#include "system.h"

const uint8_t REG_DECODEMODE = 0x09;
const uint8_t REG_INTENSITY = 0x0A;
const uint8_t REG_SCANLIMIT = 0x0B;
const uint8_t REG_SHUTDOWN = 0x0C;
int i;

void send_matrix_led(uint8_t address, uint8_t data) {
  uint8_t bit;

  // MAX7219 nháº­n 16 bit: 8 bit address, sau Ä‘Ã³ 8 bit data (MSB trÆ°á»›c)
  IOWR(MATRIX_LED_CS_BASE, 0, 0);

  for (bit = 0; bit < 8; bit++) {
    IOWR(MATRIX_LED_DIN_BASE, 0, (address & 0x80) ? 1 : 0);
    IOWR(MATRIX_LED_CLK_BASE, 0, 1);
    IOWR(MATRIX_LED_CLK_BASE, 0, 0);
    address <<= 1;
  }

  for (bit = 0; bit < 8; bit++) {
    IOWR(MATRIX_LED_DIN_BASE, 0, (data & 0x80) ? 1 : 0);
    IOWR(MATRIX_LED_CLK_BASE, 0, 1);
    IOWR(MATRIX_LED_CLK_BASE, 0, 0);
    data <<= 1;
  }

  IOWR(MATRIX_LED_CS_BASE, 0, 1);
}

void matrix_led_init(void) {
  send_matrix_led(REG_SCANLIMIT, 0x07);
  send_matrix_led(REG_DECODEMODE, 0x00);
  send_matrix_led(REG_INTENSITY, 0x03);
  send_matrix_led(REG_SHUTDOWN, 0x01);

  for (i = 1; i <= 8; i++)
    send_matrix_led((uint8_t)i, 0x00);

  //
}

static uint8_t get_font5x7_row(char ch, int row) {
  if (row < 0 || row > 6)
    return 0;

  switch (ch) {
  case 'I':
    switch (row) {
    case 0:
    case 6:
      return 0x1F;
    default:
      return 0x04;
    }
  case 'L':
    return (row == 6) ? 0x1F : 0x10;
  case 'O':
    switch (row) {
    case 0:
    case 6:
      return 0x0E;
    default:
      return 0x11;
    }
  case 'V':
    switch (row) {
    case 5:
      return 0x0A;
    case 6:
      return 0x04;
    default:
      return 0x11;
    }
  case 'E':
    switch (row) {
    case 0:
    case 3:
    case 6:
      return 0x1F;
    default:
      return 0x10;
    }
  case 'U':
    return (row == 6) ? 0x0E : 0x11;
  case 'H':
    return (row == 3) ? 0x1F : 0x11;
  default:
    return 0x00;
  }
}

static int get_text_pixel(const char *text, int text_len, int row,
                          int text_col) {
  int cell_width = 6;
  int ch_index;
  int col_in_cell;
  uint8_t row_bits;

  if (row < 0 || row > 6 || text_col < 0)
    return 0;

  ch_index = text_col / cell_width;
  if (ch_index >= text_len)
    return 0;

  col_in_cell = text_col % cell_width;
  if (col_in_cell >= 5)
    return 0;

  row_bits = get_font5x7_row(text[ch_index], row);
  return ((row_bits >> (4 - col_in_cell)) & 0x01);
}

void update_matrix_text_scroll(void) {
  static int scroll_pos = 8;
  static int tick_divider = 0;
  static const char text[] = "I LOVE IUH";
  const int text_len = (int)strlen(text);
  const int text_width = text_len * 6 - 1;
  int row, col;

  tick_divider++;
  if (tick_divider < 8)
    return;
  tick_divider = 0;

  for (row = 0; row < 8; row++) {
    uint8_t row_data = 0;

    for (col = 0; col < 8; col++) {
      int text_col = col - scroll_pos;

      if (get_text_pixel(text, text_len, row, text_col))
        row_data |= (uint8_t)(1u << (7 - col));
    }

    send_matrix_led((uint8_t)(row + 1), row_data);
  }

  scroll_pos--;
  if (scroll_pos < -text_width)
    scroll_pos = 8;
}

void clear_matrix_led() {
  int row;

  for (row = 0; row < 8; row++)
    send_matrix_led((uint8_t)(row + 1), 0x00);
}
