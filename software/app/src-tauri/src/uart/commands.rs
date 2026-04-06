use crate::state::UARTState;
use crate::uart::manager::UARTManager;
use serialport::available_ports;
use tauri::{AppHandle, State};

#[tauri::command]
pub fn list_ports() -> Result<Vec<String>, String> {
    let ports = available_ports().map_err(|e| e.to_string())?;

    Ok(ports.into_iter().map(|p| p.port_name).collect())
}

#[tauri::command]
pub fn open_port(
    app: AppHandle,
    state: State<UARTState>,
    port_name: String,
    baudrate: u32,
) -> Result<(), String> {
    let manager = UARTManager::new(state.port.clone());

    manager.open(app, port_name, baudrate)
}

#[tauri::command]
pub fn send_uart(state: State<UARTState>, data: String) -> Result<(), String> {
    let manager = UARTManager::new(state.port.clone());

    manager.send(data)
}

#[tauri::command]
pub fn close_port(app: AppHandle, state: State<UARTState>) {
    let manager = UARTManager::new(state.port.clone());

    manager.close(app);
}
