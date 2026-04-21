import { useEffect, useState } from 'react'
import { PortInfo, SerialPort } from 'tauri-plugin-serialplugin-api'

import { Button } from '@/components/ui/button'
import {
  Card,
  CardHeader,
  CardTitle,
  CardContent,
  CardFooter,
} from '@/components/ui/card'
import {
  Select,
  SelectTrigger,
  SelectValue,
  SelectContent,
  SelectItem,
} from '@/components/ui/select'
import { BaudRate, useUART } from '@/hooks/use-uart'

const loadPorts = async () => {
  const ports = await SerialPort.available_ports()
  const serializedPorts = Object.entries(ports)
    .filter(([path]) => !path.startsWith('/dev/ttyS'))
    .map(([path, info]) => ({
      ...info,
      path,
    }))

  if (serializedPorts.length === 0)
    console.warn(
      'No serial ports found. Please connect a device and refresh the page.'
    )

  return {
    ports: serializedPorts,
  }
}

export function UARTConnectCard() {
  const { status, connect, disconnect } = useUART()
  const [ports, setPorts] = useState<PortInfo[]>([])
  const [selectedPort, setSelectedPort] = useState<string | null>(null)
  const [selectedBaudRate, setSelectedBaudRate] = useState<BaudRate | null>(
    null
  )

  useEffect(() => {
    void (async () => {
      const { ports } = await loadPorts()
      setPorts(ports)
    })()
  }, [])

  return (
    <Card className='min-h-fit'>
      <CardHeader>
        <CardTitle>Available Serial Ports</CardTitle>
      </CardHeader>

      <CardContent className='flex flex-row gap-4'>
        <Select value={selectedPort} onValueChange={setSelectedPort}>
          <SelectTrigger className='flex-1'>
            <SelectValue placeholder='Select a serial port' />
          </SelectTrigger>
          <SelectContent>
            {ports.map((port) => (
              <SelectItem key={port.path} value={port.path}>
                {port.path} - {port.manufacturer || 'Unknown Manufacturer'}
              </SelectItem>
            ))}
          </SelectContent>
        </Select>
        <Select value={selectedBaudRate} onValueChange={setSelectedBaudRate}>
          <SelectTrigger>
            <SelectValue placeholder='Select a baud rate' />
          </SelectTrigger>
          <SelectContent>
            {[9600, 19200, 38400, 57600, 115200].map((baudRate) => (
              <SelectItem key={baudRate} value={baudRate.toString()}>
                {baudRate}
              </SelectItem>
            ))}
          </SelectContent>
        </Select>

        {status.type === 'connected' ? (
          <Button variant='destructive' onClick={() => disconnect()}>
            Disconnect
          </Button>
        ) : (
          <Button
            disabled={!selectedPort || !selectedBaudRate}
            onClick={() => {
              if (selectedPort && selectedBaudRate)
                connect(selectedPort, selectedBaudRate)
            }}
          >
            Connect
          </Button>
        )}
      </CardContent>

      <CardFooter className='flex flex-col items-start gap-2'>
        <p>
          Status: <span className={COLORS_MAP[status.type]}>{status.type}</span>
        </p>

        {status.type === 'error' && (
          <p className='text-destructive'>{status.message}</p>
        )}
      </CardFooter>
    </Card>
  )
}

const COLORS_MAP = {
  idle: 'text-muted-foreground',
  connecting: 'text-yellow-500',
  connected: 'text-green-500',
  error: 'text-destructive',
} as const
