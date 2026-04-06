#include "altera_avalon_timer_regs.h"
#include "system.h"

#include "timer_ctrl.h"

void timer_init(unsigned int ms)
{
    unsigned int period = (50000000 / 1000) * ms - 1;

    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE,
                                     ALTERA_AVALON_TIMER_CONTROL_STOP_MSK);

    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_BASE, period);
    IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_BASE, period >> 16);

    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE,
                                     ALTERA_AVALON_TIMER_CONTROL_CONT_MSK |
                                         ALTERA_AVALON_TIMER_CONTROL_ITO_MSK |
                                         ALTERA_AVALON_TIMER_CONTROL_START_MSK);
}

void timer_clear_timeout(void)
{
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE,
                                    ALTERA_AVALON_TIMER_STATUS_TO_MSK);
}
