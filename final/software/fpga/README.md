# FPGA Clock + Alarm

Tài liệu này giải thích toàn bộ mã nguồn hiện tại của project: luồng chạy, các module, cách set time/alarm bằng nút nhấn và UART.

## 1. Tổng quan chức năng

Project có các chức năng chính:

- Đồng hồ thời gian thực chạy mỗi giây.
- Hiển thị ngày giờ lên LCD.
- Cài đặt thời gian (`SET_TIME`) bằng nút + switch + HEX.
- Cài đặt báo thức (`SET_ALARM`) bằng nút + switch + HEX.
- Cài đặt time/alarm bằng lệnh UART.
- Kích buzzer khi đến thời điểm báo thức.

Kiểu dữ liệu trung tâm là `Date`:

- `day`, `month`, `year`
- `hour`, `minute`, `second`

---

## 2. Cấu trúc file

### `source.c`

File điều khiển chính:

- `main()` loop
- Timer ISR
- State machine `RUNNING/SET_TIME/SET_ALARM`
- Xử lý set time/alarm
- Parse lệnh UART

### `include/datetime.h`, `include/datetime.c`

Module logic thời gian:

- `datetime_tick(Date *date)`: tăng 1 giây, rollover phút/giờ/ngày/tháng/năm.
- `is_alarm(const Date *current, const Date *alarm)`: so sánh thời điểm hiện tại với báo thức.

### `include/lcd.h`, `include/lcd.c`

Driver LCD:

- Khởi tạo và bật LCD (`lcd_init`, `lcd_power_on`).
- Hiển thị ngày giờ (`lcd_show_datetime`).
- Tối ưu cập nhật: chỉ ghi phần thay đổi dựa trên `lastDate`.

### `include/timer_ctrl.h`, `include/timer_ctrl.c`

Driver timer:

- `timer_init(ms)`: cấu hình timer chu kỳ theo millisecond.
- `timer_clear_timeout()`: xóa cờ timeout trong ISR.

### `include/uart.h`, `include/uart.c`

Driver UART:

- `is_uart_available()`
- `uart_send_char`, `uart_send_string`
- `uart_receive_char`, `uart_receive_string`

---

## 3. Trạng thái hoạt động

`enum MODE` gồm:

- `RUNNING`
- `SET_TIME`
- `SET_ALARM`

Biến quan trọng trong `source.c`:

- `current_time`: thời gian hiện tại đang chạy.
- `alarm_time`: thời điểm báo thức.
- `pending_time`: bản tạm khi chỉnh time bằng switch.
- `pending_alarm`: bản tạm khi chỉnh alarm bằng switch.
- `set_time_field`, `set_alarm_field`: bước đang chỉnh (1..6).
- `alarm_counter`, `buzz_state`: điều khiển buzzer.

---

## 4. Mapping phần cứng

### Button (`BUTTON_BASE`, 3 bit)

- `BUTTON0_MASK = 0x1` (`001`): tắt buzzer.
- `BUTTON1_MASK = 0x2` (`010`): vào/chuyển bước `SET_TIME`.
- `BUTTON2_MASK = 0x4` (`100`): vào/chuyển bước `SET_ALARM`.

### Switch

- Dùng `switch[6:0]` (`IORD(SWITCH_BASE, 0) & 0x7F`).
- Giá trị được giới hạn `0..99`.

### HEX

- `HEX2`: hiển thị field đang chỉnh (`1..6`).
- `HEX1`: hàng chục.
- `HEX0`: hàng đơn vị.

### LCD

- Dòng 1: `dd/mm/yyyy`
- Dòng 2: `hh:mm:ss`

---

## 5. Luồng ngắt timer (ISR)

Hàm `Timer_IQR_Handler` làm các việc time-critical:

1. `datetime_tick(&current_time)` để đồng hồ luôn chạy nền.
2. `lcd_show_datetime(&current_time)` để cập nhật hiển thị.
3. Nếu `alarm_counter > 0`: đảo `buzz_state` để tạo nhịp buzzer.
4. `timer_clear_timeout()` để xóa cờ ngắt.

---

## 6. Luồng `main()`

Trong vòng lặp chính:

1. Nếu mode là `SET_TIME` thì gọi `set_time()`.
2. Nếu mode là `SET_ALARM` thì gọi `set_alarm()`.
3. Nếu nhấn `BUTTON1` khi `RUNNING`:
   - chờ nhả nút,
   - copy `current_time -> pending_time`,
   - reset field về `1`,
   - chuyển mode `SET_TIME`.
4. Nếu nhấn `BUTTON2` khi `RUNNING`:
   - chờ nhả nút,
   - copy `alarm_time -> pending_alarm`,
   - reset field về `1`,
   - chuyển mode `SET_ALARM`.
5. Nếu có dữ liệu UART:
   - đọc chuỗi,
   - parse command,
   - echo lại chuỗi nhận.
6. Nếu `is_alarm(current_time, alarm_time)` và chưa đang kêu:
   - đặt `alarm_counter = 10`.
7. Nếu nhấn `BUTTON0`:
   - chờ nhả nút,
   - tắt buzzer (`alarm_counter = 0`, ghi `BUZZ = 0`).

---

## 7. Cơ chế set time/alarm bằng switch + nút

`set_time()` và `set_alarm()` dùng chung lõi `run_set_mode(...)` để tránh lặp code.

Ý tưởng xử lý:

1. Đọc giá trị switch `0..99`.
2. Gán vào field hiện tại của biến pending:
   - `1`: ngày
   - `2`: tháng
   - `3`: năm (`2000 + value`)
   - `4`: giờ
   - `5`: phút
   - `6`: giây
3. Hiển thị field và giá trị lên `HEX2/HEX1/HEX0`.
4. Bắt cạnh lên của nút tương ứng:
   - chưa tới field 6 thì sang field kế,
   - ở field 6 thì commit và về `RUNNING`.

Mapping wrapper:

- `set_time()` dùng `BUTTON1`, target là `current_time`.
- `set_alarm()` dùng `BUTTON2`, target là `alarm_time`.

---

## 8. Cài đặt qua UART

Format command:

- `Tddmmyyyyhhmmss` -> set `current_time`
- `Addmmyyyyhhmmss` -> set `alarm_time`

Ví dụ:

- `T06072026080930` -> đặt thời gian thành `06/07/2026 08:09:30`

Parser UART trong `source.c`:

- `parse_2digits`, `parse_4digits`: parse số từ chuỗi.
- `parse_datetime_payload`: parse và validate payload 14 ký tự.
- `process_uart_command`: xử lý action `T` hoặc `A`.

Validate hiện có:

- `day: 1..31`
- `month: 1..12`
- `year: 0..9999`
- `hour: 0..23`
- `minute/second: 0..59`

---

## 9. Lưu ý kỹ thuật

- `datetime_tick` có xử lý năm nhuận cho tháng 2.
- LCD driver tối ưu bằng cập nhật theo phần thay đổi.
- Timer đang tính theo clock giả định 50MHz.
- Validate ngày/tháng ở UART hiện là mức cơ bản, chưa check sâu theo từng tháng cụ thể.

---

## 10. Tóm tắt nhanh

- ISR: tick đồng hồ + buzzer + clear timeout.
- Main loop: mode control + button + UART + alarm check.
- Set mode: tái sử dụng `run_set_mode` cho cả time và alarm.
- UART: hỗ trợ set nhanh bằng chuỗi `T/A + ddmmyyyyhhmmss`.
