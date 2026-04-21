import { useState, useEffect } from 'react'

import { Button } from '@/components/ui/button'
import { Card, CardHeader, CardTitle, CardContent } from '@/components/ui/card'
import { useUART } from '@/hooks/use-uart'

export function AlarmStatusCard() {
  const { status, send, subscribe } = useUART()
  const [isActive, setIsActive] = useState<boolean>(false)

  useEffect(() => {
    if (status.type !== 'connected') return

    const unsubscribe = subscribe((data) => {
      if (data.startsWith('A')) setIsActive(data[1] === '1')
    })

    return () => void unsubscribe.then((unsub) => unsub?.())
  }, [status, subscribe])

  return (
    <Card className='min-h-fit'>
      <CardHeader>
        <CardTitle>Alarm Status</CardTitle>
      </CardHeader>
      <CardContent className='flex flex-row items-center justify-between gap-4'>
        <p className='text-lg'>
          Alarm is{' '}
          {isActive ? (
            <span className='text-green-500'>active</span>
          ) : (
            <span className='text-red-500'>inactive</span>
          )}
          .
        </p>

        <Button
          size='sm'
          variant='destructive'
          disabled={status.type !== 'connected' || !isActive}
          onClick={() => send('S')}
        >
          Turn off alarm
        </Button>
      </CardContent>
    </Card>
  )
}
