#include "datetime.h"
#include "utils.h"

void DateTime_init(struct DateTime *dt, int year, int month, int day, int hour,
                   int minute, int second) {
  dt->year = year;
  dt->month = month;
  dt->day = day;
  dt->hour = hour;
  dt->minute = minute;
  dt->second = second;
}

void DateTime_increment(struct DateTime *dt) {
  dt->second += 1;

  if (dt->second >= 60) {
    dt->second = 0;
    dt->minute += 1;
  }

  if (dt->minute >= 60) {
    dt->minute = 0;
    dt->hour += 1;
  }

  if (dt->hour >= 24) {
    dt->hour = 0;
    dt->day += 1;
  }

  if (dt->day > days_in_month(dt->month, dt->year)) {
    dt->day = 1;
    dt->month += 1;
  }

  if (dt->month > 12) {
    dt->month = 1;
    dt->year += 1;
  }
}
