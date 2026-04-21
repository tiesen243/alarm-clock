#ifndef UTILS_H
#define UTILS_H

#include "datetime.h"

int set_datetime(struct DateTime *target, const char *label, int hex_0_pin,
                 int hex_1_pin, int button_pin, int switch_pin);

int is_alarm(struct DateTime *current, struct DateTime *alarm);

int is_leap_year(int year);

int days_in_month(int month, int year);

int parse_uart_command(const char *cmd, struct DateTime *dt);

#endif // !UTILS_H
