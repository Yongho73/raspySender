#include "main.h"

int isBlank(char ch)
{
    return ((ch == 0x20) || (ch == '\t') || (ch == '\r') || (ch == '\n'));
}

char *trim(char *s)
{
    char *f = s, *e = 0, *c = s;

    /* 뒤쪽 공백 제거 */
    e = s + (strlen(s)) - 1;
    while (isBlank(*e) && s <= e)
        e--;
    *(e + 1) = 0;

    /* 앞쪽 공백 제거 */
    while (isBlank(*f) && f <= e)
        f++;

    /* 공백 없는 부분 복사 */
    if (s != f)
    {
        while (f <= e)
            *(c++) = *(f++);
        *c = 0;
    }
    return s;
}

int setConfig(CONFIG *conf, char *confFile)
{
    char readBuff[512];
    char varBuff[256];
    char *temp;

    FILE *fp;

    memset(&varBuff, 0x00, sizeof(varBuff));

    fp = fopen(confFile, "r");

    if (fp)
    {
        while (!feof(fp))
        {
            memset(&readBuff, 0, sizeof(readBuff));
            if (fgets(readBuff, 512, fp) == NULL)
            {
                continue;
            }
            temp = trim(readBuff);
            if (*temp == '#')
            {
                continue;
            }
            if (strncasecmp(temp, "deviceName", strlen("deviceName")) == 0)
            {
                strcpy(conf->deviceName, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "deviceNo", strlen("deviceNo")) == 0)
            {
                strcpy(conf->deviceNo, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "deviceGPS", strlen("deviceGPS")) == 0)
            {
                strcpy(conf->deviceGPS, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "deviceType", strlen("deviceType")) == 0)
            {
                strcpy(conf->deviceType, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "exhaustType", strlen("exhaustType")) == 0)
            {
                strcpy(conf->exhaustType, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "signalGpioPort", strlen("signalGpioPort")) == 0)
            {
                strcpy(conf->signalGpioPort, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "sensorReadDevice", strlen("sensorReadDevice")) == 0)
            {
                strcpy(conf->sensorReadDevice, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "mqttHostIp", strlen("mqttHostIp")) == 0)
            {
                strcpy(conf->mqttHostIp, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "mqttPort", strlen("mqttPort")) == 0)
            {
                strcpy(conf->mqttPort, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "mqttUser", strlen("mqttUser")) == 0)
            {
                strcpy(conf->mqttUser, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "mqttPswd", strlen("mqttPswd")) == 0)
            {
                strcpy(conf->mqttPswd, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "apiHost", strlen("apiHost")) == 0)
            {
                strcpy(conf->apiHost, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "apiPort", strlen("apiPort")) == 0)
            {
                strcpy(conf->apiPort, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "logPath", strlen("logPath")) == 0)
            {
                strcpy(conf->logPath, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "delaySecond", strlen("delaySecond")) == 0)
            {
                strcpy(conf->delaySecond, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "cipherSK", strlen("cipherSK")) == 0)
            {
                strcpy(conf->cipherSK, strrchr(temp, '=') + 1);
            }
            if (strncasecmp(temp, "cipherIV", strlen("cipherIV")) == 0)
            {
                strcpy(conf->cipherIV, strrchr(temp, '=') + 1);
            }
        }
    }
    fclose(fp);
    return 1;
}
