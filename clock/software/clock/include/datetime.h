#ifndef DATETIME_H
#define DATETIME_H

struct DateTime {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
};

void DateTime_init(struct DateTime *dt, int year, int month, int day, int hour,
                   int minute, int second);

void DateTime_increment(struct DateTime *dt);

#endif // !DATETIME_H
