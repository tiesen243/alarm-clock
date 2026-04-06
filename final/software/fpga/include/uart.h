#ifndef UART_H
#define UART_H

int is_uart_available(void);

void uart_send_char(char c);
void uart_send_string(const char *str);

char uart_receive_char(void);
void uart_receive_string(char *buffer, int max_length);

#endif
