import { useState, useEffect } from 'react'

import { Button } from '@/components/ui/button'
import {
  Card,
  CardHeader,
  CardTitle,
  CardAction,
  CardContent,
} from '@/components/ui/card'
import { useUART } from '@/hooks/use-uart'

export function UARTLogCard() {
  const [logs, setLogs] = useState<string[]>([])
  const { status, subscribe } = useUART()

  useEffect(() => {
    if (status.type !== 'connected') return

    const unsubscribe = subscribe((data) => {
      setLogs((prevLogs) => [...prevLogs, data])
    })

    return () => void unsubscribe.then((unsub) => unsub?.())
  }, [status, subscribe])

  return (
    <Card className='max-h-max overflow-hidden'>
      <CardHeader>
        <CardTitle>UART Logs</CardTitle>
        <CardAction>
          <Button variant='outline' size='sm' onClick={() => setLogs([])}>
            Clear Logs
          </Button>
        </CardAction>
      </CardHeader>
      <CardContent className='flex flex-col gap-2 overflow-y-auto'>
        {logs.map((log, index) => (
          <p key={index} className='text-sm'>
            {log}
          </p>
        ))}
      </CardContent>
    </Card>
  )
}
