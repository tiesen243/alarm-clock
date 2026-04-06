import { listen } from '@tauri-apps/api/event'
import { useCallback, useEffect, useRef } from 'react'

export function useUARTSubscription(callback: (data: string) => void) {
  const bufferRef = useRef<string>('')

  const clean = useCallback(() => {
    bufferRef.current = ''
  }, [])

  useEffect(() => {
    let unlisten: (() => void) | undefined
    let isMounted = true

    const setupListener = async () => {
      const fn = await listen<string>('uart-data', (event) => {
        const sanitized = event.payload.replace(/[^\x20-\x7E\r\n\t]/g, '')
        bufferRef.current += sanitized

        let newlineIndex: number
        while ((newlineIndex = bufferRef.current.indexOf('\n')) !== -1) {
          const line = bufferRef.current
            .substring(0, newlineIndex)
            .replace('\r', '')
          bufferRef.current = bufferRef.current.substring(newlineIndex + 1)

          if (line) {
            const now = Intl.DateTimeFormat('en-US', {
              day: '2-digit',
              month: '2-digit',
              year: '2-digit',
              hour: '2-digit',
              minute: '2-digit',
              second: '2-digit',
              hour12: false,
            }).format(new Date())

            callback(`[${now}] ${line}`)
          }
        }
      })

      if (!isMounted) fn()
      else unlisten = fn
    }

    setupListener()

    return () => {
      isMounted = false
      if (unlisten) unlisten()
    }
  }, [callback])

  return { clean }
}
