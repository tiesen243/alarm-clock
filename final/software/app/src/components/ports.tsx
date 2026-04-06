import { RefreshCcwIcon } from 'lucide-react'
import { useState } from 'react'

import { Button } from '@/components/ui/button'
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from '@/components/ui/select'
import { useUART } from '@/hooks/use-uart'

export const Ports: React.FC = () => {
  const { ports, status, reloadPorts, connect, disconnect } = useUART()
  const [port, setPort] = useState<string | null>(null)
  const [baudrate, setBaudrate] = useState<number>(9600)

  return (
    <section className='flex flex-col gap-2'>
      <h2 className='scroll-m-20 pb-2 text-3xl font-semibold tracking-tight first:mt-0 flex-1'>
        Connection Settings
      </h2>

      <section className='flex items-center gap-2'>
        <h3 className='sr-only'>Select a port</h3>

        <Select
          value={port}
          onValueChange={setPort}
          disabled={status.type === 'Connected'}
        >
          <SelectTrigger className='flex-1'>
            <SelectValue placeholder='Select a port' />
          </SelectTrigger>

          <SelectContent>
            {ports.map((port) => (
              <SelectItem key={port} value={port}>
                {port}
              </SelectItem>
            ))}
          </SelectContent>
        </Select>

        <Select
          value={baudrate.toString()}
          onValueChange={(value) => setBaudrate(Number(value))}
          disabled={status.type === 'Connected'}
        >
          <SelectTrigger className='w-32'>
            <SelectValue placeholder='Baudrate' />
          </SelectTrigger>

          <SelectContent>
            {baudrates.map((rate) => (
              <SelectItem key={rate} value={rate.toString()}>
                {rate}
              </SelectItem>
            ))}
          </SelectContent>
        </Select>

        <Button
          variant='outline'
          size='icon'
          onClick={reloadPorts}
          disabled={status.type === 'Connected'}
        >
          <RefreshCcwIcon />
          <span className='sr-only'>Reload ports</span>
        </Button>

        {status.type === 'Connected' ? (
          <Button variant='destructive' onClick={disconnect}>
            Disconnect
          </Button>
        ) : (
          <Button onClick={() => connect(port, baudrate)}>Connect</Button>
        )}
      </section>

      <p>
        Status: <strong>{status.type}</strong>
        {status.type === 'Error' && (
          <>
            <br /> Cause: <em className='text-destructive'>{status.data}</em>
          </>
        )}
      </p>
    </section>
  )
}

const baudrates = [
  300, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200,
]
