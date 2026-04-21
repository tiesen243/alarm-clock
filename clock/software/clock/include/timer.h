#ifndef TIMER_H
#define TIMER_H

#include "altera_avalon_timer_regs.h"
#include "system.h"

void timer_init(int pin, int period_ms);

void timer_cleanup(int pin);

#endif // !TIMER_H
