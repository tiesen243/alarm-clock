#include "uart.h"
#include "system.h"

int is_uart_available() {
  return (IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE) &
          ALTERA_AVALON_UART_STATUS_RRDY_MSK) != 0;
}

void uart_send_byte(char byte) {
  while (!(IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE) &
           ALTERA_AVALON_UART_STATUS_TRDY_MSK))
    ;
  IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, byte);
}

void uart_send_string(const char *str) {
  while (*str) {
    uart_send_byte(*str);
    str++;
  }

  uart_send_byte('\n');
}

char uart_receive_byte() {
  while (!is_uart_available(UART_0_BASE))
    ;
  return IORD_ALTERA_AVALON_UART_RXDATA(UART_0_BASE);
}

void uart_receive_string(char *buffer, int buffer_size) {
  if (buffer_size <= 0)
    return;

  int index = 0;
  while (index < buffer_size - 1) {
    char byte = uart_receive_byte(UART_0_BASE);
    if (byte == '\n' || byte == '\r')
      break;
    buffer[index++] = byte;
  }

  buffer[index] = '\0';
}
