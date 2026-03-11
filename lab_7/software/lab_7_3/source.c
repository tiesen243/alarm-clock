/*
 * source.c
 *
 *  Created on: Mar 11, 2026
 *      Author: tiesen243
 */

#include <stdio.h>
#include "system.h"
#include "altera_avalon_uart_regs.h"

void send(char *str)
{
    while (*str)
    {
        while (!(IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE) & ALTERA_AVALON_UART_STATUS_TRDY_MSK))
            ;
        IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, *str++);
    }


    while (!(IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE) & ALTERA_AVALON_UART_STATUS_TRDY_MSK))
        ;
    IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, '\n');
}

char toUpper(char c) {
    if (c >= 'a' && c <= 'z') return c - 32;
    return c;
}

int main()
{
    char rxData;
    printf("Start\n");

    while (1)
    {
        if ((IORD_ALTERA_AVALON_UART_STATUS(UART_0_BASE) & 0x80) != 0x80)
            continue;

        rxData = toUpper(IORD_ALTERA_AVALON_UART_RXDATA(UART_0_BASE));

        if (rxData == 'S') send("Down");
        else if (rxData == 'W') send("Up");
        else if (rxData == 'A') send("Left");
        else if (rxData == 'D') send("Right");
    }

    return 0;
}
