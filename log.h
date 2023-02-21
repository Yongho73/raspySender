#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <ftw.h>
#include <wiringPi.h>
#include <stdbool.h>

void makelog(char *format, ...);                                /* 로그 생성 */
void makeDatalog(char *type, char *format, ...);                /* 오류시 로그 생성용 */
void makeBackuplog(char *type, char *format, ...);              /* 로그용 */
void makeRestorelog(char *type, bool stamp, char *format, ...); /* 복구용*/