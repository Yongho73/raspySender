#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include "main.h"

int initDelaySecond = 60;
int initWaitRestore = 0;
int initWaitWork = 0;

int work()
{
    CONFIG conf;
    memset(&conf, 0x00, sizeof(CONFIG));
    setConfig(&conf, CONFIG_PATH);

    char timer[4];
    memset(&timer, 0x00, sizeof(timer));

    int nowTime = 0;
    int befTime = 99;
    int delaySecond = atoi(conf.delaySecond);

    do
    {
        nowTime = (unsigned)time(NULL); // 초단위
        if (((nowTime % 60) % delaySecond) == 0)
        {
            if (nowTime != befTime)
            {
                befTime = nowTime;
                sprintf(timer, "%02d", (nowTime % 60));
                // 처리할 내용
                makelog("[%s][%s][%d][%d]", timer, "runing...", delaySecond, ((nowTime % 60) % delaySecond));
                run(timer);
                makelog("-----------------------------------------------------------------------------");
            }
        }
        delay(100);
    } while (1);

    return 0;
}

// 1년전 로그 삭제
int yearFileRemove(const char *path, const struct stat *sb, int typeflag)
{
    // 1년전 오늘
    char oneYearPreDay[30];
    memset(&oneYearPreDay, 0x00, sizeof(oneYearPreDay));
    strcpy(oneYearPreDay, timestemp('Z'));

    // 파일 생성 일자
    char dateStr[30];
    memset(&dateStr, 0x00, sizeof(dateStr));
    time_t mtime = sb->st_mtime;
    struct tm *tblock = localtime(&mtime);
    sprintf(dateStr, "%4d%02d%02d", tblock->tm_year + 1900,
            tblock->tm_mon + 1,
            tblock->tm_mday);

    // send_ 로 시작하는 파일만
    int position_send = strstr(path, "send_") - path;

    // 파일만
    if (typeflag == 0 && position_send >= 0)
    {
        if (atoi(dateStr) < atoi(oneYearPreDay))
        {
            makeDatalog("data", "year file delete! path=[%s], position_send=[%d], typeflag=[%d], dateStr=[%d], oneYearPreDay=[%d]", path, position_send, typeflag, atoi(dateStr), atoi(oneYearPreDay));
            remove(path);
        }
    }

    return 0;
}

// 일주일전 파일 삭제
int weekFileRemove(const char *path, const struct stat *sb, int typeflag)
{
    // 7일전
    char oneWeekPreDay[30];
    memset(&oneWeekPreDay, 0x00, sizeof(oneWeekPreDay));
    strcpy(oneWeekPreDay, timestemp('W'));

    // 파일 생성 일자
    char dateStr[30];
    memset(&dateStr, 0x00, sizeof(dateStr));
    time_t mtime = sb->st_mtime;
    struct tm *tblock = localtime(&mtime);
    sprintf(dateStr, "%4d%02d%02d", tblock->tm_year + 1900,
            tblock->tm_mon + 1,
            tblock->tm_mday);

    // send_ 로 시작하는 파일만
    int position_log = strstr(path, "log_") - path;
    int position_data = strstr(path, "data_") - path;

    // 파일만
    if (typeflag == 0 && (position_log >= 0 || position_data >= 0))
    {
        if (atoi(dateStr) < atoi(oneWeekPreDay))
        {
            makeDatalog("data", "log delete! path=[%s], position_log=[%d], position_data=[%d], typeflag=[%d], dateStr=[%d], oneWeekPreDay=[%d]", path, position_log, position_data, typeflag, atoi(dateStr), atoi(oneWeekPreDay));
            remove(path);
        }
    }

    return 0;
}

// 하루전 파일 삭제
int oneDayFileRemove(const char *path, const struct stat *sb, int typeflag)
{
    // 1일전
    char oneWeekPreDay[30];
    memset(&oneWeekPreDay, 0x00, sizeof(oneWeekPreDay));
    strcpy(oneWeekPreDay, timestemp('O'));

    // 파일 생성 일자
    char dateStr[30];
    memset(&dateStr, 0x00, sizeof(dateStr));
    time_t mtime = sb->st_mtime;
    struct tm *tblock = localtime(&mtime);
    sprintf(dateStr, "%4d%02d%02d", tblock->tm_year + 1900,
            tblock->tm_mon + 1,
            tblock->tm_mday);

    // send_ 로 시작하는 파일만
    int position_done = strstr(path, "done_") - path;

    // 파일만
    if (typeflag == 0 && position_done >= 0)
    {
        if (atoi(dateStr) <= atoi(oneWeekPreDay))
        {
            makeDatalog("data", "done delete! path=[%s], position_done=[%d], typeflag=[%d], dateStr=[%d], oneWeekPreDay=[%d]", path, position_done, typeflag, atoi(dateStr), atoi(oneWeekPreDay));
            remove(path);
        }
    }

    return 0;
}

int restore()
{

    CONFIG conf;
    memset(&conf, 0x00, sizeof(CONFIG));
    setConfig(&conf, CONFIG_PATH);

    char oneYearPreDate[15];

    int nowTime = 0;
    int befTime = 0;

    do
    {
        memset(&oneYearPreDate, 0x00, sizeof(oneYearPreDate));
        strcpy(oneYearPreDate, timestemp('P'));

        nowTime = (unsigned)time(NULL);
        if (((nowTime % 60) % 55) == 0 && (nowTime % 60) != 0)
        {
            if (nowTime != befTime)
            {
                befTime = nowTime;
                makeDatalog("data", "[%d][%s] ======--------*", (nowTime % 60), "runing...");
                // 1년전 로그 삭제
                makeDatalog("data", "> year file deleted! logPath=[%s]", conf.logPath);
                ftw(conf.logPath, yearFileRemove, FTW_DNR);
                // 일주일전 로그 삭제
                makeDatalog("data", "> week file deleted! logPath=[%s]", conf.logPath);
                ftw(conf.logPath, weekFileRemove, FTW_DNR);
                // 하루전 로그 삭제
                makeDatalog("data", "> one day file deleted! logPath=[%s]", conf.logPath);
                ftw(conf.logPath, oneDayFileRemove, FTW_DNR);
                // 1년전 데이터 삭제
                makeDatalog("data", "> sqlite deleted! logPath=[%s]", conf.logPath);
                sendDataDelete(oneYearPreDate);                
                // 전송 실패건 재전송
                makeDatalog("data", "> restore start ======--------*");
                readReady();

                makeDatalog("data", "-----------------------------------------------------------------------------");
            }
        }
        delay(100);
    } while (1);

    return 0;
}

int workProcess()
{
    int pid;
    int ret;
    int cid;

    pid = fork();
    if (pid < 0)
    {
        makelog("workProcess Main fork Error... : return is [%d]", pid);
        return 0;
    }
    else if (pid > 0)
    {
        return 0;
    }
    else
    {
        makelog("workProcess process : [%d]", getpid());
    }

    signal(SIGHUP, SIG_IGN);
    setsid();
    open("/dev/null", O_RDWR);

    // 초기 프로그램 시작시 안정화 될때까지 기다림
    if (initWaitWork == 0)
    {
        sleep(initDelaySecond);
        initWaitWork = 1;
    }

    while (1)
    {
        pid = fork();
        if (pid < 0)
        {
            makelog("workProcess Sub fork Error... : return is [%d]", pid);
            return 0;
        }
        else if (pid == 0)
        {
            break;
        }
        else if (pid > 0)
        {
            cid = wait(&ret);
            makelog("workProcess Error occured... : return is [%d][%x]", cid, ret);
        }
    }

    // 메인 실행
    work();
    return 0;
}

int restoreProcess()
{
    int pid;
    int ret;
    int cid;

    pid = fork();
    if (pid < 0)
    {
        makelog("restoreProcess Main fork Error... : return is [%d]", pid);
        return 0;
    }
    else if (pid > 0)
    {
        return 0;
    }
    else
    {
        makelog("restoreProcess process : [%d]", getpid());
    }

    signal(SIGHUP, SIG_IGN);
    setsid();
    open("/dev/null", O_RDWR);

    // 초기 프로그램 시작시 안정화 될때까지 기다림
    if (initWaitRestore == 0)
    {
        sleep(initDelaySecond);
        initWaitRestore = 1;
    }

    while (1)
    {
        pid = fork();
        if (pid < 0)
        {
            makelog("restoreProcess Sub fork Error... : return is [%d]", pid);
            return 0;
        }
        else if (pid == 0)
        {
            break;
        }
        else if (pid > 0)
        {
            cid = wait(&ret);
            makelog("restoreProcess Error occured... : return is [%d][%x]", cid, ret);
        }
    }

    // 메인 실행
    restore();
    return 0;
}

int main(int argc, char *argv[])
{
    makeDatabase();   // 데이터베이스 생성
    workProcess();    // 메인 프로세스
    restoreProcess(); // 복원 프로세스
    return 0;
}
