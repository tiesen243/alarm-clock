import '@/globals.css'

import { ThemeProvider } from 'next-themes'
import React from 'react'
import ReactDOM from 'react-dom/client'

import App from '@/app'
import { UARTProvider } from '@/hooks/use-uart'

ReactDOM.createRoot(document.getElementById('root') as HTMLElement).render(
  <React.StrictMode>
    <ThemeProvider
      attribute='class'
      defaultTheme='system'
      disableTransitionOnChange
    >
      <UARTProvider>
        <App />
      </UARTProvider>
    </ThemeProvider>
  </React.StrictMode>
)
