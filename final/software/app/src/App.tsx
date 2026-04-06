import { useState } from "react";
import "./App.css";

import reactLogo from "./assets/react.svg";
import { useUART } from "./hooks/use-uart";
import { useUARTSubscription } from "./hooks/use-uart-subscription";

function App() {
  const { ports, status, connect, disconnect, send } = useUART();
  const [port, setPort] = useState<string>(ports[0] ?? "");
  const [data, setData] = useState<string[]>([]);
  const [msg, setMsg] = useState("");

  useUARTSubscription((data) => {
    console.log("Received data:", data);
    return setData((prev) => [...prev, data]);
  });

  return (
    <main className="container">
      <h1>Welcome to Tauri + React</h1>

      <div className="row">
        <a href="https://vite.dev" target="_blank">
          <img src="/vite.svg" className="logo vite" alt="Vite logo" />
        </a>
        <a href="https://tauri.app" target="_blank">
          <img src="/tauri.svg" className="logo tauri" alt="Tauri logo" />
        </a>
        <a href="https://react.dev" target="_blank">
          <img src={reactLogo} className="logo react" alt="React logo" />
        </a>
      </div>
      <p>Click on the Tauri, Vite, and React logos to learn more.</p>

      <div style={styles.field}>
        <select
          style={styles.fieldSelect}
          value={port}
          onChange={(e) => {
            console.log(e);

            return setPort(e.target.value);
          }}
        >
          {ports.map((port) => (
            <option key={port}>{port}</option>
          ))}
        </select>

        {status.type === "Connected" ? (
          <button onClick={() => disconnect()}>Disconnect</button>
        ) : (
          <button onClick={() => connect(port, 9600)}>Connect</button>
        )}
      </div>

      <p>{JSON.stringify(status)}</p>

      <div>
        <input
          type="text"
          value={msg}
          onChange={(e) => setMsg(e.target.value)}
          placeholder="Type a message to send"
        />
        <button onClick={() => send(msg)}>Send</button>
      </div>

      <pre>{JSON.stringify(data, null, 2)}</pre>
    </main>
  );
}

export default App;

const styles = {
  field: {
    display: "flex",
    flexDirection: "row",
    alignItems: "center",
    gap: "0,5rem",
  },

  fieldSelect: {
    flex: 1,
  },
} as Record<string, React.CSSProperties>;
