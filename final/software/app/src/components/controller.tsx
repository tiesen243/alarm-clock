import { useState } from 'react'

import { Button } from '@/components/ui/button'
import {
  Card,
  CardContent,
  CardDescription,
  CardFooter,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'
import { Input } from '@/components/ui/input'
import { useUART } from '@/hooks/use-uart'
import { useUARTSubscription } from '@/hooks/use-uart-subscription'

export const Controller: React.FC = () => {
  return (
    <section className='flex flex-col gap-2'>
      <h2 className='scroll-m-20 pb-2 text-3xl font-semibold tracking-tight first:mt-0 flex-1'>
        Controller
      </h2>

      <section className='grid grid-cols-1 md:grid-cols-2 gap-4'>
        <Set type='Time' />

        <Set type='Alarm' />

        <AlarmStatus />
      </section>
    </section>
  )
}

export const Set: React.FC<{ type: 'Time' | 'Alarm' }> = ({ type }) => {
  const { send } = useUART()
  const [value, setValue] = useState<string>(() => {
    const tzDate = new Date(
      new Date().toLocaleString('en-US', { timeZone: 'Asia/Ho_Chi_Minh' })
    )
    const yyyy = tzDate.getFullYear()
    const mm = String(tzDate.getMonth() + 1).padStart(2, '0')
    const dd = String(tzDate.getDate()).padStart(2, '0')
    const hh = String(tzDate.getHours()).padStart(2, '0')
    const min = String(tzDate.getMinutes()).padStart(2, '0')
    const ss = String(tzDate.getSeconds()).padStart(2, '0')
    return `${yyyy}-${mm}-${dd}T${hh}:${min}:${ss}`
  })

  const handleSubmit = () => {
    if (!value) return

    let command = type === 'Time' ? 'T' : 'A'
    const date = new Date(value)
    command += date.getDate().toString().padStart(2, '0')
    command += (date.getMonth() + 1).toString().padStart(2, '0')
    command += date.getFullYear().toString()
    command += date.getHours().toString().padStart(2, '0')
    command += date.getMinutes().toString().padStart(2, '0')
    command += date.getSeconds().toString().padStart(2, '0')
    send(command)
  }

  return (
    <Card>
      <CardHeader>
        <CardTitle>Set {type}</CardTitle>
        <CardDescription>
          Configure the {type.toLowerCase()} for the alarm clock.
          {value}
        </CardDescription>
      </CardHeader>

      <CardContent>
        <Input
          type='datetime-local'
          step={1}
          value={value}
          onChange={(e) => setValue(e.target.value)}
        />
      </CardContent>

      <CardFooter className='justify-end'>
        <Button onClick={handleSubmit}>Set {type}</Button>
      </CardFooter>
    </Card>
  )
}

const AlarmStatus: React.FC = () => {
  const { send } = useUART()
  const [isActive, setIsActive] = useState<boolean>(false)

  useUARTSubscription((data) => {
    if (data.startsWith('A')) setIsActive(data[1] === '1')
  })

  return (
    <Card className='col-span-full'>
      <CardHeader>
        <CardTitle>Alarm Status</CardTitle>
        <CardDescription>
          View the current status of the alarm clock, including whether the
          alarm is active and the current time. This information is updated in
          real-time
        </CardDescription>
      </CardHeader>

      <CardFooter className='justify-between'>
        <p>
          Alarm is currently{' '}
          <span
            className={
              isActive
                ? 'text-green-500 font-semibold'
                : 'text-red-500 font-semibold'
            }
          >
            {isActive ? 'active' : 'inactive'}
          </span>
        </p>

        <Button
          variant={isActive ? 'destructive' : 'outline'}
          onClick={() => send('S')}
          disabled={!isActive}
        >
          Turn Off Alarm
        </Button>
      </CardFooter>
    </Card>
  )
}
