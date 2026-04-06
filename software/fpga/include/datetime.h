#ifndef DATETIME_H
#define DATETIME_H

typedef struct
{
    int day;
    int month;
    int year;

    int hour;
    int minute;
    int second;
} Date;

void datetime_tick(Date *date);
int is_alarm(const Date *current, const Date *alarm);

#endif
