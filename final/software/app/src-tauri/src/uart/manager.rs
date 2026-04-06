use crate::uart::status::UARTStatus;
use serialport::SerialPort;
use std::{
    io::{Read, Write},
    sync::{Arc, Mutex},
    thread,
    time::Duration,
};
use tauri::{AppHandle, Emitter};

pub struct UARTManager {
    port: Arc<Mutex<Option<Box<dyn SerialPort>>>>,
}

impl UARTManager {
    pub fn new(port: Arc<Mutex<Option<Box<dyn SerialPort>>>>) -> Self {
        Self { port }
    }

    pub fn open(&self, app: AppHandle, port_name: String, baudrate: u32) -> Result<(), String> {
        app.emit("uart-status", UARTStatus::Connecting).ok();

        let port = serialport::new(port_name, baudrate)
            .timeout(Duration::from_millis(100))
            .open()
            .map_err(|e| {
                app.emit("uart-status", UARTStatus::Error(e.to_string()))
                    .ok();
                e.to_string()
            })?;

        {
            let mut guard = self.port.lock().unwrap();
            *guard = Some(port);
        }

        app.emit("uart-status", UARTStatus::Connected).ok();

        self.start_reader(app);

        Ok(())
    }

    fn start_reader(&self, app: AppHandle) {
        let port_clone = self.port.clone();

        thread::spawn(move || {
            let mut buffer = [0u8; 1024];

            loop {
                let mut guard = port_clone.lock().unwrap();

                if let Some(ref mut port) = *guard {
                    match port.read(&mut buffer) {
                        Ok(n) if n > 0 => {
                            let data = String::from_utf8_lossy(&buffer[..n]).to_string();

                            app.emit("uart-data", data).ok();
                        }

                        _ => {}
                    }
                }

                drop(guard);

                thread::sleep(Duration::from_millis(10));
            }
        });
    }

    pub fn send(&self, data: String) -> Result<(), String> {
        let mut guard = self.port.lock().unwrap();

        if let Some(ref mut port) = *guard {
            port.write_all(data.as_bytes()).map_err(|e| e.to_string())?;

            Ok(())
        } else {
            Err("Port not connected".into())
        }
    }

    pub fn close(&self, app: AppHandle) {
        let mut guard = self.port.lock().unwrap();

        *guard = None;

        app.emit("uart-status", UARTStatus::Disconnected).ok();
    }
}
