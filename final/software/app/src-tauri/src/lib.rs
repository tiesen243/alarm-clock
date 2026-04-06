mod state;
mod uart;

use state::UARTState;
use uart::commands::{close_port, list_ports, open_port, send_uart};

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_opener::init())
        .manage(UARTState::new())
        .invoke_handler(tauri::generate_handler![
            list_ports, open_port, send_uart, close_port
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
