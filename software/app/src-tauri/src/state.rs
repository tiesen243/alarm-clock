use serialport::SerialPort;
use std::sync::{Arc, Mutex};

pub struct UARTState {
    pub port: Arc<Mutex<Option<Box<dyn SerialPort>>>>,
}

impl UARTState {
    pub fn new() -> Self {
        Self {
            port: Arc::new(Mutex::new(None)),
        }
    }
}
