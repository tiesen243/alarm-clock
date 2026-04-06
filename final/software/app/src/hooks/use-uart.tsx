import { invoke } from '@tauri-apps/api/core'
import { listen } from '@tauri-apps/api/event'
import * as React from 'react'

type UARTStatus =
  | { type: 'Connecting' }
  | { type: 'Connected' }
  | { type: 'Disconnected' }
  | { type: 'Error'; data: string }

interface UARTContextValue {
  ports: string[]
  status: UARTStatus
  reloadPorts: () => Promise<unknown>
  connect: (port: string | null, baudrate?: number) => Promise<unknown>
  disconnect: () => Promise<unknown>
  send: (data: string) => Promise<unknown>
}

const UARTContext = React.createContext<UARTContextValue | null>(null)

const useUART = () => {
  const context = React.use(UARTContext)
  if (!context) throw new Error('useUART must be used within a UARTProvider')
  return context
}

function UARTProvider({ children }: Readonly<{ children: React.ReactNode }>) {
  const [ports, setPorts] = React.useState<string[]>([])
  const [status, setStatus] = React.useState<UARTStatus>({
    type: 'Disconnected',
  })

  const connect = React.useCallback(
    async (port: string | null, baudrate = 9600) => {
      if (!port) return

      return await invoke('open_port', { portName: port, baudrate })
    },
    []
  )

  const disconnect = React.useCallback(async () => {
    return await invoke('close_port')
  }, [])

  const send = React.useCallback(async (data: string) => {
    return await invoke('send_uart', { data })
  }, [])

  const reloadPorts = React.useCallback(
    async () => invoke<string[]>('list_ports').then(setPorts),
    []
  )

  React.useEffect(() => {
    reloadPorts()
    listen<UARTStatus>('uart-status', (event) => setStatus(event.payload))
  }, [])

  const value = React.useMemo(
    () => ({
      ports,
      status,
      reloadPorts,
      connect,
      disconnect,
      send,
    }),
    [ports, status]
  )

  return <UARTContext value={value}>{children}</UARTContext>
}

export { UARTProvider, useUART }
