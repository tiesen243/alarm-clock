#include "timer.h"

void timer_init(int pin, int period_ms) {
  unsigned int period = 0;
  IOWR_ALTERA_AVALON_TIMER_CONTROL(pin, ALTERA_AVALON_TIMER_CONTROL_STOP_MSK);

  period = (period_ms * (500000 / 1000)) - 1;
  IOWR_ALTERA_AVALON_TIMER_PERIODL(pin, period);
  IOWR_ALTERA_AVALON_TIMER_PERIODH(pin, period >> 16);

  IOWR_ALTERA_AVALON_TIMER_CONTROL(pin,
                                   ALTERA_AVALON_TIMER_CONTROL_CONT_MSK |
                                       ALTERA_AVALON_TIMER_CONTROL_ITO_MSK |
                                       ALTERA_AVALON_TIMER_CONTROL_START_MSK);
}

void timer_cleanup(int pin) {
  IOWR_ALTERA_AVALON_TIMER_STATUS(pin, ALTERA_AVALON_TIMER_STATUS_TO_MSK);
}
