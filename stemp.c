#include "main.h"

char *timestemp(char tpy)
{
    static char dateStr[30];
    memset(&dateStr, 0x00, sizeof(dateStr));

    time_t timer;
    struct tm *tblock;

    if (tpy == 'T')
    {
        timer = time(NULL);
        tblock = localtime(&timer);
        sprintf(dateStr, "%4d%02d%02d%02d%02d%02d", tblock->tm_year + 1900,
                tblock->tm_mon + 1,
                tblock->tm_mday,
                tblock->tm_hour,
                tblock->tm_min,
                tblock->tm_sec);
    }
    else if (tpy == 'D')
    {
        timer = time(NULL);
        tblock = localtime(&timer);
        sprintf(dateStr, "%4d%02d%02d", tblock->tm_year + 1900,
                tblock->tm_mon + 1,
                tblock->tm_mday);
    }
    else if (tpy == 'S')
    {
        timer = time(NULL);
        tblock = localtime(&timer);
        sprintf(dateStr, "%02d", tblock->tm_sec);
    }
    else if (tpy == 'H')
    {
        timer = time(NULL);
        tblock = localtime(&timer);
        sprintf(dateStr, "%4d%02d%02d%02d%02d", tblock->tm_year + 1900,
                tblock->tm_mon + 1,
                tblock->tm_mday,
                tblock->tm_hour,
                tblock->tm_min);
    }
    else if (tpy == 'P')
    {
        timer = time(NULL) - (24 * 60 * 60 * 365);
        tblock = localtime(&timer);
        sprintf(dateStr, "%4d%02d%02d%02d%02d%02d", tblock->tm_year + 1900,
                tblock->tm_mon + 1,
                tblock->tm_mday,
                tblock->tm_hour,
                tblock->tm_min,
                tblock->tm_sec);
    }
    else if (tpy == 'Z')
    {
        timer = time(NULL) - (24 * 60 * 60 * 365);
        tblock = localtime(&timer);
        sprintf(dateStr, "%4d%02d%02d", tblock->tm_year + 1900,
                tblock->tm_mon + 1,
                tblock->tm_mday);
    }
    else if (tpy == 'W')
    {
        timer = time(NULL) - (24 * 60 * 60 * 7);
        tblock = localtime(&timer);
        sprintf(dateStr, "%4d%02d%02d", tblock->tm_year + 1900,
                tblock->tm_mon + 1,
                tblock->tm_mday);
    }
    else if (tpy == 'Y')
    {
        timer = time(NULL) - (24 * 60 * 60 * 365);
        tblock = localtime(&timer);
        sprintf(dateStr, "%4d", tblock->tm_year + 1900);
    }
    else if (tpy == 'O')
    {
        timer = time(NULL) - (24 * 60 * 60 * 1);
        tblock = localtime(&timer);
        sprintf(dateStr, "%4d%02d%02d", tblock->tm_year + 1900,
                tblock->tm_mon + 1,
                tblock->tm_mday);
    }
    else
    {
        timer = time(NULL);
        tblock = localtime(&timer);
        sprintf(dateStr, "%02d:%02d:%02d",
                tblock->tm_hour,
                tblock->tm_min,
                tblock->tm_sec);
    }

    return dateStr;
}
