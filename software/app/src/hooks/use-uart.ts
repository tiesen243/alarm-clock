import * as React from "react";
import { invoke } from "@tauri-apps/api/core";
import { listen } from "@tauri-apps/api/event";

type UARTStatus =
  | { type: "Connecting" }
  | { type: "Connected" }
  | { type: "Disconnected" }
  | { type: "Error"; data: string };

export function useUART() {
  const [ports, setPorts] = React.useState<string[]>([]);
  const [status, setStatus] = React.useState<UARTStatus>({ type: "Disconnected" });

  const connect = async (port: string, baudrate = 9600) => {
    return await invoke("open_port", {
      portName: port,
      baudrate,
    });
  };

  const disconnect = async () => {
    return await invoke("close_port");
  };

  const send = async (data: string) => {
    return await invoke("send_uart", { data });
  };

  React.useEffect(() => {
    invoke<string[]>("list_ports").then(setPorts);
    listen<UARTStatus>("uart-status", (event) => {
      setStatus(event.payload);
    });
  }, []);

  return { ports, status, connect, disconnect, send };
}
