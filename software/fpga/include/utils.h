#ifndef UTILS_H
#define UTILS_H

#include "datetime.h"

const int HEX_7SEG[16] = {
    0x40, // 0
    0x79, // 1
    0x24, // 2
    0x30, // 3
    0x19, // 4
    0x12, // 5
    0x02, // 6
    0x78, // 7
    0x00, // 8
    0x10, // 9
    0x08, // A
    0x03, // b
    0x46, // C
    0x21, // d
    0x06, // E
    0x0E  // F
};

int run_set_datetime(Date *target, const char *done_label);
int parse_uart_time_command(const char *cmd, Date *out);

#endif // !#ifndef UTILS_H
