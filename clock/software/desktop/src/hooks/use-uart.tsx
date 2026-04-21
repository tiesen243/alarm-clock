import * as React from 'react'
import { SerialPort } from 'tauri-plugin-serialplugin-api'

type UARTStatus =
  | { type: 'idle' }
  | { type: 'connecting' }
  | { type: 'connected'; port: SerialPort }
  | { type: 'error'; message: string }

export const baudRates = ['9600', '19200', '38400', '57600', '115200'] as const
export type BaudRate = (typeof baudRates)[number]

interface UARTContextValue {
  status: UARTStatus
  connect: (port: string | null, baudRate?: BaudRate) => Promise<void>
  disconnect: () => Promise<void>
  subscribe: (callback: (data: string) => void) => Promise<() => void>
  send: (message: string) => Promise<void>
}

const UARTContext = React.createContext<UARTContextValue | undefined>(undefined)

const useUART = (): UARTContextValue => {
  const context = React.useContext(UARTContext)
  if (!context) throw new Error('useUART must be used within a UARTProvider')
  return context
}

function UARTProvider({ children }: { children: React.ReactNode }) {
  const [status, setStatus] = React.useState<UARTStatus>({ type: 'idle' })

  const connect = React.useCallback(
    async (port: string | null, baudRate: BaudRate = '9600') => {
      if (status.type === 'connected' || !port) return

      setStatus({ type: 'connecting' })
      const connectedPort = new SerialPort({
        path: port ?? '',
        baudRate: Number.parseInt(baudRate ?? '9600', 10),
      })

      try {
        await connectedPort.open()
        setStatus({ type: 'connected', port: connectedPort })
      } catch (error) {
        setStatus({
          type: 'error',
          message: error instanceof Error ? error.message : String(error),
        })
      }
    },
    [status]
  )

  const disconnect = React.useCallback(async () => {
    if (status.type !== 'connected') return
    await status.port.close()
    setStatus({ type: 'idle' })
  }, [status])

  const send = React.useCallback(
    async (message: string) => {
      if (status.type !== 'connected') return
      await status.port.write(message)
    },
    [status]
  )

  const subscribe = React.useCallback(
    // oxlint-disable-next-line promise/prefer-await-to-callbacks
    async (callback: (data: string) => void) => {
      if (status.type !== 'connected') return

      await status.port.startListening()
      await status.port.listen(callback)
      return () => status.port.stopListening()
    },
    [status]
  )

  const value = React.useMemo(
    () => ({
      status,
      connect,
      disconnect,
      subscribe,
      send,
    }),
    [status, connect, disconnect, subscribe, send]
  ) as UARTContextValue

  return <UARTContext value={value}>{children}</UARTContext>
}

export { UARTProvider, useUART }
