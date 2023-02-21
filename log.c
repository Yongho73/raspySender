#include "main.h"

void makelog(char *format, ...)
{
    CONFIG conf;
    memset(&conf, 0x00, sizeof(CONFIG));
    setConfig(&conf, CONFIG_PATH);

    va_list arg_list;
    FILE *fp;

    char logPath[500];
    memset(&logPath, 0x00, sizeof(logPath));
    strcat(logPath, conf.logPath);
    strcat(logPath, "/log_");
    strcat(logPath, timestemp('D'));
    //strcat(logPath, conf.deviceNo);

    /*
    // 전날 생성 로그 삭제    
    struct stat sb;
    stat(logPath, &sb);
    char fcdateStr[30];
    memset(&fcdateStr, 0x00, sizeof(fcdateStr));

    char todateStr[30];
    memset(&todateStr, 0x00, sizeof(todateStr));

    struct tm *tblock;
    tblock = localtime(&sb.st_atime);
    sprintf(fcdateStr, "%4d%02d%02d", tblock->tm_year + 1900, tblock->tm_mon + 1, tblock->tm_mday);
    strcpy(todateStr, timestemp('D'));

    if (strcmp(fcdateStr, todateStr) != 0)
    {
        remove(logPath);
    }
    */

    // 로그 생성
    char dmsg[500];
    char tmsg[1000];
    memset(&dmsg, 0x00, sizeof(dmsg));
    memset(&tmsg, 0x00, sizeof(tmsg));

    strcat(tmsg, timestemp('M'));
    strcat(tmsg, "=>");

    va_start(arg_list, format);
    vsprintf(dmsg, format, arg_list);
    va_end(arg_list);

    // 시간=>내용 합침
    strcat(tmsg, dmsg);

    fp = fopen(logPath, "a+");
    fputs(tmsg, fp);
    fputs("\n", fp);
    fclose(fp);
}

void makeDatalog(char *type, char *format, ...)
{
    CONFIG conf;
    memset(&conf, 0x00, sizeof(CONFIG));
    setConfig(&conf, CONFIG_PATH);

    va_list arg_list;
    FILE *fp;

    char logPath[500];
    memset(&logPath, 0x00, sizeof(logPath));
    strcat(logPath, conf.logPath);
    strcat(logPath, "/");
    strcat(logPath, type);
    strcat(logPath, "_");
    strcat(logPath, timestemp('D'));
    //strcat(logPath, conf.deviceNo);

    /*
    // 전날 생성 로그 삭제
    struct stat sb;
    stat(logPath, &sb);
    char fcdateStr[30];
    memset(&fcdateStr, 0x00, sizeof(fcdateStr));

    char todateStr[30];
    memset(&todateStr, 0x00, sizeof(todateStr));

    struct tm *tblock;
    tblock = localtime(&sb.st_atime);
    sprintf(fcdateStr, "%4d%02d%02d", tblock->tm_year + 1900, tblock->tm_mon + 1, tblock->tm_mday);
    strcpy(todateStr, timestemp('D'));

    if (strcmp(fcdateStr, todateStr) != 0)
    {
        remove(logPath);
    }
    */

    // 로그 생성
    char dmsg[500];
    memset(&dmsg, 0x00, sizeof(dmsg));

    va_start(arg_list, format);
    vsprintf(dmsg, format, arg_list);
    va_end(arg_list);

    fp = fopen(logPath, "a+");
    fputs(dmsg, fp);
    fputs("\n", fp);
    fclose(fp);
}

void makeBackuplog(char *type, char *format, ...)
{
    CONFIG conf;
    memset(&conf, 0x00, sizeof(CONFIG));
    setConfig(&conf, CONFIG_PATH);

    va_list arg_list;
    FILE *fp;

    char logPath[500];
    memset(&logPath, 0x00, sizeof(logPath));

    char *backupDate = timestemp('D');

    strcat(logPath, conf.logPath);
    strcat(logPath, "/send_data/");
    strcat(logPath, type);
    strcat(logPath, "_");
    strcat(logPath, backupDate);

    // 로그 생성
    char dmsg[1000];
    memset(&dmsg, 0x00, sizeof(dmsg));

    va_start(arg_list, format);
    vsprintf(dmsg, format, arg_list);
    va_end(arg_list);

    fp = fopen(logPath, "a+");
    fputs(dmsg, fp);
    fputs("\n", fp);
    fclose(fp);
}

void makeRestorelog(char *type, bool stamp, char *format, ...)
{
    CONFIG conf;
    memset(&conf, 0x00, sizeof(CONFIG));
    setConfig(&conf, CONFIG_PATH);

    va_list arg_list;
    FILE *fp;

    char logPath[500];
    memset(&logPath, 0x00, sizeof(logPath));

    strcat(logPath, conf.logPath);
    strcat(logPath, "/restore/");
    strcat(logPath, type);
    
    // 타임스템프 적용
    if(stamp){
        char *backupDate = timestemp('T');
        strcat(logPath, "_");
        strcat(logPath, backupDate);
    }

    // 로그 생성
    char dmsg[1000];
    memset(&dmsg, 0x00, sizeof(dmsg));

    va_start(arg_list, format);
    vsprintf(dmsg, format, arg_list);
    va_end(arg_list);

    fp = fopen(logPath, "a+");
    fputs(dmsg, fp);
    fputs("\n", fp);
    fclose(fp);
}
