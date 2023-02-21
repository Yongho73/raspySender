#pragma once
#include <sqlite3.h>

typedef struct
{
    char m1_co2[100];
    char m1_o2[100];
    char m1_ch4[100];
    char m1_afm[100];
    char m2_co2[100];
    char m2_o2[100];
    char m2_ch4[100];
    char m2_afm[100];
    char temp[100];
    char humi[100];
    char timer[20];
    char success[2];
} CCD_DATA;

int makeDatabase();                                                                                                                                                  /* 데이터베이스 생성 */
int sendDataInsert(long in_co2, long in_o2, long in_ch4, long in_afm, long out_co2, long out_o2, long out_ch4, long out_afm, int st_temp, int st_humi, char *timer); /* 데이터 입력 */
int sendDataDelete(char *timer);                                                                                                                                     /* 1년전 데이터 삭제 */
