import { useState } from 'react'

import { Button } from '@/components/ui/button'
import { Card, CardHeader, CardTitle, CardContent } from '@/components/ui/card'
import { Input } from '@/components/ui/input'
import { useUART } from '@/hooks/use-uart'

export function SetTimeCard({ type }: { type: 'time' | 'alarm' }) {
  const [time, setTime] = useState<string>(
    new Date(Date.now() + 7 * 60 * 60 * 1000).toISOString().slice(0, 19)
  )

  const { status, send } = useUART()

  return (
    <Card>
      <CardHeader>
        <CardTitle>Set {type}</CardTitle>
      </CardHeader>
      <CardContent className='flex flex-col gap-4'>
        <Input
          type='datetime-local'
          step={1}
          value={time}
          onChange={(e) => setTime(e.target.value)}
        />

        <Button
          disabled={status.type !== 'connected'}
          onClick={() => {
            const timestamp = new Date(time)

            let cmd = type === 'time' ? 'T' : 'A'
            cmd += timestamp.getFullYear().toString().padStart(4, '0')
            cmd += (timestamp.getMonth() + 1).toString().padStart(2, '0')
            cmd += timestamp.getDate().toString().padStart(2, '0')
            cmd += timestamp.getHours().toString().padStart(2, '0')
            cmd += timestamp.getMinutes().toString().padStart(2, '0')
            cmd += timestamp.getSeconds().toString().padStart(2, '0')

            send(cmd)
          }}
        >
          Set {type}
        </Button>
      </CardContent>
    </Card>
  )
}
