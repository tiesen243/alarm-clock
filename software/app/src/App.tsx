import { ThemeProvider } from 'next-themes'

import { Controller } from '@/components/controller'
import { Logs } from '@/components/logs'
import { Ports } from '@/components/ports'
import { UARTProvider } from '@/hooks/use-uart'

export default function App() {
  return (
    <ThemeProvider
      attribute='class'
      defaultTheme='system'
      enableSystem
      disableTransitionOnChange
    >
      <UARTProvider>
        <main className='container mx-auto p-4 flex flex-col gap-6'>
          <h1 className='scroll-m-20 text-center text-4xl font-extrabold tracking-tight text-balance'>
            UART Clock Controller
          </h1>

          <Ports />

          <Controller />

          <Logs />
        </main>
      </UARTProvider>
    </ThemeProvider>
  )
}
