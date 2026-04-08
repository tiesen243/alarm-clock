import { useEffect, useRef, useState } from 'react'

import { Button } from '@/components/ui/button'
import { Item, ItemContent, ItemGroup, ItemTitle } from '@/components/ui/item'
import { ScrollArea } from '@/components/ui/scroll-area'
import { useUARTSubscription } from '@/hooks/use-uart-subscription'

export const Logs: React.FC = () => {
  const [logs, setLogs] = useState<string[]>([])
  const bottomRef = useRef<HTMLDivElement>(null)

  useUARTSubscription((data) => {
    setLogs((prev) => [...prev, data])
  })

  useEffect(() => {
    if (bottomRef.current)
      bottomRef.current.scrollIntoView({ behavior: 'smooth' })
  }, [logs])


  return (
    <section className='flex flex-col gap-2'>
      <div className='flex items-center justify-between pb-2'>
        <h2 className='scroll-m-20 text-3xl font-semibold tracking-tight first:mt-0 flex-1'>
          Logs
        </h2>

        <Button variant='outline' size='sm' onClick={() => setLogs([])}>
          Clear Logs
        </Button>
      </div>

      <ScrollArea className='h-64 w-full'>
        <ItemGroup>
          {logs.map((log, index) => (
            <Item variant='outline' key={`${log}-${index}`}>
              <ItemContent>
                <ItemTitle>{log}</ItemTitle>
              </ItemContent>
            </Item>
          ))}
        </ItemGroup>
        <div ref={bottomRef} />
      </ScrollArea>
    </section>
  )
}
