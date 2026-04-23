#include "timer.h"
#include "system.h"

void timer_init(int period_ms) {
  unsigned int period = 0;
  IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,
                                   ALTERA_AVALON_TIMER_CONTROL_STOP_MSK);

  period = (period_ms * (500000 / 1000)) - 1;
  IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, period);
  IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, period >> 16);

  IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,
                                   ALTERA_AVALON_TIMER_CONTROL_CONT_MSK |
                                       ALTERA_AVALON_TIMER_CONTROL_ITO_MSK |
                                       ALTERA_AVALON_TIMER_CONTROL_START_MSK);
}

void timer_cleanup() {
  IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE,
                                  ALTERA_AVALON_TIMER_STATUS_TO_MSK);
}
