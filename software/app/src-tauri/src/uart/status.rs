use serde::Serialize;

#[derive(Clone, Serialize)]
#[serde(tag = "type", content = "data")]
pub enum UARTStatus {
    Connecting,
    Connected,
    Disconnected,
    Error(String),
}
