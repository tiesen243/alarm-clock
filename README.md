# Đồng hồ

## Tổng quan

Đồng hồ được viết bằng FPGA (verilog) và Nios II để hiển thị thời gian, đặt báo thức, và có thể điều chỉnh thời gian thông qua nút nhấn và phần mềm.

## Tính năng

- Hiển thị thời gian hiện tại (ngày, tháng, năm, giờ, phút, giây).
- Đặt báo thức với thời gian cụ thể.
- Điều chỉnh thời gian thông qua nút nhấn.
- Giao diện người dùng đơn giản và dễ sử dụng.

## Phần cứng

| Pin           | Chức năng                          |
| ------------- | ---------------------------------- |
| CLOCK_50      | Clock 50MHz                        |
| UART_RXD      | UART Receive Data                  |
| UART_TXD      | UART Transmit Data                 |
| LCD_ON        | LCD On/Off Control                 |
| LCD_BLON      | LCD Backlight Control              |
| LCD_EN        | LCD Enable                         |
| LCD_RW        | LCD Read/Write Control             |
| LCD_RS        | LCD Register Select                |
| LCD_DATA[7:0] | LCD Data Bus                       |
| GPIO[3:0]     | Nút nhấn (để điều khiển chức năng) |
| SW[6:0]       | Công tắc (để đặt giờ)              |
| GPIO[4]       | Matrix LED Data In                 |
| GPIO[5]       | Matrix LED Chip Select             |
| GPIO[6]       | Matrix LED Clock                   |
| GPIO[8]       | Buzzer Control                     |
| LEDR[17:0]    | LED Output                         |
| HEX0-HEX7     | 7-segment Display Output           |

## Cách cài đặt

1. Mở dự án `fpga.qpf` trong Quartus II 13.0sp1.
2. Chọn device "Cyclone IV E" (EP4CE115F29C7).
3. Nhấn "Assignments" -> "Import Assignments" và chọn file `DE2_115_pin_assignments.csv` để thiết lập chân kết nối.
4. Nhấn "Tools" -> "Qysys" mở file `system.qsys` và nhấn "Generate" để tạo các file cần thiết.
5. Quay lại Quartus, nhấn "Add Files" và thêm "fpga.v" và "./system/synthesis/system.qip" vào dự án.
6. Nhấn "Compile" để biên dịch dự án.
7. Sau khi biên dịch thành công, nhấn "Program" để nạp chương trình vào FPGA.
8. Nhấn "Tools" -> "Nios II Software Build Tools for Eclipse" để mở môi trường phát triển phần mềm.
9. Tạo một project mới trong Eclipse và chọn "Nios II Application and BSP from Template".
10. Chọn đúng file "system.sopcinfo" và template "Blank".
11. Copy mã nguồn từ `./software/fpga/source.c` và `./software/fpga/include` vào project của bạn.
12. Chuột phải vào project_bsp -> "Nios II BSP" -> "BSP Editor..." và chọn:
    - "enable_reduced_device_drivers" để giảm kích thước BSP.
    - "enable_small_c_library" để sử dụng thư viện C nhỏ hơn.
    - sys_clk_timer -> "none" để tắt timer mặc định.
    - "timestamp_timer" -> "none" để tắt timer timestamp.
    - Nhấn "Generate" để cập nhật BSP.
13. Build project và nạp chương trình vào Nios II.
    - Lưu ý: Cần tick chọn "Ignore mismatched system ID" và "Ignore mismatched system timestamp" khi nạp chương trình để tránh lỗi do hệ thống đã được cập nhật sau khi biên dịch FPGA.
14. Sau khi nạp thành công, bạn có thể sử dụng phần mềm để điều chỉnh thời gian và đặt báo thức.

## Cách sử dụng

1. Điều chỉnh thời gian:
   - Nhấn nút GPIO[1] để chọn chức năng điều chỉnh thời gian.
   - Sử dụng SW[6:0] để điều chỉnh giá trị 0->99
   - Nhấn GPIO[3] để chuyển sang phần tiếp theo (năm -> tháng -> ngày -> giờ -> phút -> giây).
   - Nhấn GPIO[3] lần nữa để lưu thời gian đã điều chỉnh.
2. Đặt báo thức:
   - Nhấn nút GPIO[2] để chọn chức năng đặt báo thức.
   - Sử dụng SW[6:0] để điều chỉnh như trên để đặt thời gian báo thức.
   - Có thể nhấn GPIO[0] để tắt báo thức khi nó kêu.

   Khi báo thức kêu, buzzer sẽ phát âm thanh, matrix led sẽ hiển thị "I Love IUH", led[17:0] sẽ sáng đuổi từ phải sang trái

3. Điều khiển bằng phần mềm:
   - Cần kết nối UART của kit với máy tính để có thể gửi lệnh điều khiển qua Serial.
   - Cài đặt phần mềm tại [đây](https://github.com/tiesen243/422001423701-soc/releases/tag/v0.0.4).
   - Mở phần mềm, chọn cổng COM tương ứng và Baudrate 9600 và nhấn "Connect".
   - Sử dụng các nút trên giao diện phần mềm để điều chỉnh thời gian, đặt báo thức, hoặc tắt báo thức khi nó kêu.

## Kết luận

Dự án đồng hồ sử dụng FPGA và Nios II đã được hoàn thành với đầy đủ các tính năng như hiển thị thời gian, đặt báo thức, và điều chỉnh thời gian thông qua nút nhấn và phần mềm. Đây là một dự án phức tạp nhưng rất hữu ích để học về thiết kế hệ thống nhúng và lập trình FPGA.
