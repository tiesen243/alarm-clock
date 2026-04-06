#include "datetime.h"

void datetime_tick(Date *date)
{
    date->second++;

    if (date->second >= 60)
    {
        date->second = 0;
        date->minute++;
    }

    if (date->minute >= 60)
    {
        date->minute = 0;
        date->hour++;
    }

    if (date->hour >= 24)
    {
        date->hour = 0;
        date->day++;
    }

    if (date->month == 2)
    {
        int isLeapYear = (date->year % 4 == 0 && date->year % 100 != 0) || (date->year % 400 == 0);
        int daysInMonth = isLeapYear ? 29 : 28;

        if (date->day > daysInMonth)
        {
            date->day = 1;
            date->month++;
        }
    }
    else if (date->month == 4 || date->month == 6 || date->month == 9 || date->month == 11)
    {
        if (date->day > 30)
        {
            date->day = 1;
            date->month++;
        }
    }
    else
    {
        if (date->day > 31)
        {
            date->day = 1;
            date->month++;
        }
    }

    if (date->month > 12)
    {
        date->month = 1;
        date->year++;
    }
}

int is_alarm(const Date *current, const Date *alarm)
{
    return current->day == alarm->day &&
           current->month == alarm->month &&
           current->year == alarm->year &&
           current->hour == alarm->hour &&
           current->minute == alarm->minute &&
           current->second == alarm->second;
}
