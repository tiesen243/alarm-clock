#ifndef UTILS_H
#define UTILS_H

#include "datetime.h"

int run_set_datetime(Date *target, const char *done_label);
int parse_uart_time_command(const char *cmd, Date *out);

#endif // !#ifndef UTILS_H
