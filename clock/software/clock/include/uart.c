#include "uart.h"

int is_uart_available(int uart_base) {
  return (IORD_ALTERA_AVALON_UART_STATUS(uart_base) &
          ALTERA_AVALON_UART_STATUS_RRDY_MSK) != 0;
}

void uart_send_byte(int uart_base, char byte) {
  while (!(IORD_ALTERA_AVALON_UART_STATUS(uart_base) &
           ALTERA_AVALON_UART_STATUS_TRDY_MSK))
    ;
  IOWR_ALTERA_AVALON_UART_TXDATA(uart_base, byte);
}

void uart_send_string(int uart_base, const char *str) {
  while (*str) {
    uart_send_byte(uart_base, *str);
    str++;
  }

  uart_send_byte(uart_base, '\n');
}

char uart_receive_byte(int uart_base) {
  while (!is_uart_available(uart_base))
    ;
  return IORD_ALTERA_AVALON_UART_RXDATA(uart_base);
}

void uart_receive_string(int uart_base, char *buffer, int buffer_size) {
  if (buffer_size <= 0)
    return;

  int index = 0;
  while (index < buffer_size - 1) {
    char byte = uart_receive_byte(uart_base);
    if (byte == '\n' || byte == '\r')
      break;
    buffer[index++] = byte;
  }

  buffer[index] = '\0';
}
