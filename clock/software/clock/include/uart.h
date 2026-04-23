#ifndef UART_H
#define UART_H

#include "altera_avalon_uart_regs.h"

#define UART_RX_IDLE_LIMIT 50000

int is_uart_available();

void uart_send_byte(char byte);

void uart_send_string(const char *str);

char uart_receive_byte();

void uart_receive_string(char *buffer, int buffer_size);

#endif // !UART_H
