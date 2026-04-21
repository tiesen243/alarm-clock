#ifndef UART_H
#define UART_H

#include "altera_avalon_uart_regs.h"

#define UART_RX_IDLE_LIMIT 50000

int is_uart_available(int uart_base);

void uart_send_byte(int uart_base, char byte);

void uart_send_string(int uart_base, const char *str);

char uart_receive_byte(int uart_base);

void uart_receive_string(int uart_base, char *buffer, int buffer_size);

#endif // !UART_H
