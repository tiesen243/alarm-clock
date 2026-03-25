/*
 * source.c
 *
 *  Created on: Mar 18, 2026
 *      Author: tiesen243
 */

#include <stdio.h>
#include "altera_avalon_dma_regs.h"
#include "system.h"

#define DMA_TRANSFER_SIZE_BYTES 32

volatile unsigned char *pdata0 = (volatile unsigned char *)ONCHIP_MEMORY2_0_BASE;
volatile unsigned char *pdata1 = (volatile unsigned char *)ONCHIP_MEMORY2_1_BASE;

void Init_Source_And_Destination(void) // tạo dữ liệu nguồn và xóa dữ liệu đích trước khi thực hiện DMA transfer
{
    int i;

    for (i = 0; i < DMA_TRANSFER_SIZE_BYTES; i++)
    {
        pdata0[i] = (unsigned char)i;
        pdata1[i] = 0;
    }
}

void Clear_Destination(void) // xóa dữ liệu đích trước khi thực hiện DMA transfer
{
    int i;

    for (i = 0; i < DMA_TRANSFER_SIZE_BYTES; i++)
    {
        pdata1[i] = 0;
    }
}

void Print_Destination(const char *label) // in dữ liệu đích sau khi thực hiện DMA transfer
{
    int i;

    printf("%s\n", label);
    for (i = 0; i < DMA_TRANSFER_SIZE_BYTES; i++)
    {
        printf("byte %d: %u\n", i, (unsigned int)pdata1[i]);
    }
}

void DMA_Transfer(unsigned int transfer_mode) // thực hiện DMA transfer với chế độ truyền được chỉ định (halfword hoặc word)
{
    IOWR_ALTERA_AVALON_DMA_STATUS(DMA_0_BASE, ALTERA_AVALON_DMA_STATUS_DONE_MSK);
    IOWR_ALTERA_AVALON_DMA_CONTROL(DMA_0_BASE, 0);

    IOWR_ALTERA_AVALON_DMA_RADDRESS(DMA_0_BASE, (int)pdata0);
    IOWR_ALTERA_AVALON_DMA_WADDRESS(DMA_0_BASE, (int)pdata1);

    IOWR_ALTERA_AVALON_DMA_LENGTH(DMA_0_BASE, DMA_TRANSFER_SIZE_BYTES);

    IOWR_ALTERA_AVALON_DMA_CONTROL(DMA_0_BASE, transfer_mode |
                                                   ALTERA_AVALON_DMA_CONTROL_BYTE_MSK |
                                                   ALTERA_AVALON_DMA_CONTROL_LEEN_MSK |
                                                   ALTERA_AVALON_DMA_CONTROL_I_EN_MSK |
                                                   ALTERA_AVALON_DMA_CONTROL_GO_MSK);

    while ((IORD_ALTERA_AVALON_DMA_STATUS(DMA_0_BASE) & ALTERA_AVALON_DMA_STATUS_DONE_MSK) == 0)
        ;

    IOWR_ALTERA_AVALON_DMA_STATUS(DMA_0_BASE, ALTERA_AVALON_DMA_STATUS_DONE_MSK);
}

int main(void)
{
    Init_Source_And_Destination();

    printf("DMA halfword transfer: 32 bytes from onchip_memory2_0 to onchip_memory2_1\n");
    DMA_Transfer(ALTERA_AVALON_DMA_CONTROL_HW_MSK); // thực hiện DMA transfer với chế độ halfword
    Print_Destination("Result after halfword transfer:");

    Clear_Destination();

    printf("DMA word transfer: 32 bytes from onchip_memory2_0 to onchip_memory2_1\n");
    DMA_Transfer(ALTERA_AVALON_DMA_CONTROL_WORD_MSK);
    Print_Destination("Result after word transfer:");

    while (1)
        ;

    return 0;
}
