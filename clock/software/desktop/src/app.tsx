import { AlarmStatusCard } from '@/components/alarm-status'
import { SetTimeCard } from '@/components/set-time'
import { UARTConnectCard } from '@/components/uart-connect'
import { UARTLogCard } from '@/components/uart-log'

function App() {
  return (
    <main className='container mx-auto flex flex-col gap-4 p-4 md:max-h-screen md:overflow-hidden'>
      <UARTConnectCard />

      <section className='grid grid-cols-1 gap-4 md:grid-cols-2'>
        <SetTimeCard type='time' />
        <SetTimeCard type='alarm' />
      </section>

      <AlarmStatusCard />

      <UARTLogCard />
    </main>
  )
}

export default App
