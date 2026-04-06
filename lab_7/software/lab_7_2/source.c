/*
 * source.c
 *
 *  Created on: Mar 11, 2026
 *      Author: tiesen243
 */

#include <stdio.h>
#include <unistd.h>
#include "system.h"
#include "sys/alt_stdio.h"
#include "altera_avalon_uart_regs.h"
#include "altera_avalon_pio_regs.h"

int counter = 0;
char number[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};

int main()
{
	printf("Start\n");

	while (1)
	{
		if (IORD(BUTTON_BASE, 0) == 1)
		{
			while (IORD(BUTTON_BASE, 0) == 1)
				;
			counter++;
			if (counter > 99)
				counter = 0;

			printf("Counter: %d\n", counter);
			IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, number[counter / 10]);
			usleep(1);
			IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, number[counter % 10]);
			usleep(1);
			IOWR_ALTERA_AVALON_UART_TXDATA(UART_0_BASE, '\n');
			usleep(1);
		}
	}

	return 0;
}
