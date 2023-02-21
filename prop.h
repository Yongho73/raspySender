#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct
{
    char deviceName[20];
    char deviceNo[15];
    char deviceGPS[50];
    char deviceType[5];
    char exhaustType[5];
    char signalGpioPort[3];
    char sensorReadDevice[15];
    char mqttHostIp[20];
    char mqttPort[10];
    char mqttUser[20];
    char mqttPswd[100];
    char apiHost[50];
    char apiPort[10];
    char logPath[200];
    char delaySecond[10];
    char cipherSK[40];
    char cipherIV[20];
} CONFIG;

int isBlank(char ch);                        /* 공백문자 여부 */
char *trim(char *s);                         /* 문자열 트림 */
int setConfig(CONFIG *conf, char *confFile); /* config 파일 읽기 */
