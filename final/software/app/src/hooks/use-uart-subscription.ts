import { useEffect, useRef } from "react";
import { listen } from "@tauri-apps/api/event";

export function useUARTSubscription(callback: (data: string) => void) {
  const bufferRef = useRef<string>("");

  useEffect(() => {
    let unlisten: () => void;

    listen<string>("uart-data", (event) => {
      bufferRef.current += event.payload;

      let newlineIndex: number;
      while ((newlineIndex = bufferRef.current.indexOf("\n")) !== -1) {
        const line = bufferRef.current.substring(0, newlineIndex).replace("\r", "");
        bufferRef.current = bufferRef.current.substring(newlineIndex + 1);

        if (line) callback(line);
      }
    }).then((fn) => {
      unlisten = fn;
    });

    return () => {
      if (unlisten) unlisten();
    };
  }, [callback]);
}
