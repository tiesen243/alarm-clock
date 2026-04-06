#include <stdio.h>
#include "system.h"
#include "altera_avalon_dma_regs.h"
#include "sys/alt_irq.h"

// 1. Dùng kiểu int để chứa mảng dữ liệu 32-bit (Word)
int pdata0[32] = {0,1,2,3,4,5,6,7,8,9,10,
                  11,12,13,14,15,16,17,18,19,20,
                  21,22,23,24,25,26,27,28,29,30,31};

// 2. Ép kiểu con trỏ đích sang int* (Word)
int* pdata1 = (int*)(ONCHIP_MEMORY2_1_BASE);

// Interrupt handler of DMA (Trình phục vụ ngắt)
void DMA_ISR_Handler(void* isr_context){
    int i;

    // Đọc và in dữ liệu từ onchip_memory2_1
    for(i=0; i<32; i++){
        printf("word %d = %d\n", i, pdata1[i]); // In ra dữ liệu Word
    }

    // Xóa cờ ngắt DMA (Ghi 0 vào thanh ghi status)
    IOWR_ALTERA_AVALON_DMA_STATUS(DMA_0_BASE, 0);
}

// Initialize function of DMA
void DMA_Init(void){

    // Dừng DMA (Reset control)
    IOWR_ALTERA_AVALON_DMA_CONTROL(DMA_0_BASE, 0);
    // Nên xóa cờ trạng thái/ngắt cũ trước khi cấu hình vòng chạy mới
    IOWR_ALTERA_AVALON_DMA_STATUS(DMA_0_BASE, 0);

    // Đặt địa chỉ nguồn là pdata0
    IOWR_ALTERA_AVALON_DMA_RADDRESS(DMA_0_BASE, (int)pdata0);

    // Đặt địa chỉ đích là pdata1
    IOWR_ALTERA_AVALON_DMA_WADDRESS(DMA_0_BASE, (int)pdata1);

    // 3. Sửa Length: 32 phần tử * 4 bytes/phần tử = 128 bytes
    IOWR_ALTERA_AVALON_DMA_LENGTH(DMA_0_BASE, 32);

    // 4. Configure and Start DMA (Dùng cờ WORD_MSK)
    IOWR_ALTERA_AVALON_DMA_CONTROL(DMA_0_BASE,
          ALTERA_AVALON_DMA_CONTROL_WORD_MSK |       // Truyền Word (32-bit)
          ALTERA_AVALON_DMA_CONTROL_LEEN_MSK |       // Dừng khi length đếm ngược về 0
          ALTERA_AVALON_DMA_CONTROL_I_EN_MSK |       // Cho phép ngắt
          ALTERA_AVALON_DMA_CONTROL_GO_MSK);         // Khởi động DMA (Bắt đầu chạy)
}

// entry point main()
int main(void){

    // 5. Bắt buộc Đăng ký ngắt TRƯỚC khi cấu hình và chạy DMA
    alt_ic_isr_register(0, DMA_0_IRQ, DMA_ISR_Handler, (void*)0, (void*)0);

    // Bắt đầu cấu hình và cho DMA hoạt động
    DMA_Init();

    // Vòng lặp vô tận, chương trình chính tạm dừng để chờ ngắt DMA
    while(1);

    return 0;
}
