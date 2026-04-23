#ifndef TIMER_H
#define TIMER_H

#include "altera_avalon_timer_regs.h"
#include "system.h"

void timer_init(int period_ms);

void timer_cleanup();

#endif // !TIMER_H
