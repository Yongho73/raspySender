#include "main.h"

char logPath[500];

int makeDatabase()
{
    CONFIG conf;
    memset(&conf, 0x00, sizeof(CONFIG));
    setConfig(&conf, CONFIG_PATH);

    // 데이터베이스 로그 위치
    memset(&logPath, 0x00, sizeof(logPath));
    strcat(logPath, conf.logPath);
    strcat(logPath, "/database/send_data.db");

    sqlite3 *db_send_data;
    sqlite3_stmt *res_send_data;
    int rc_send_data = 0;
    char *err_send_data = 0;
    char *createSql_send_data = "create table if not exists send_data (m1_co2 integer, m1_o2 integer, m1_ch4 integer, m1_afm integer, m2_co2 integer, m2_o2 integer, m2_ch4 integer, m2_afm integer, temp integer, humi integer, timer text); "
                                "create index if not exists idx_send_data on send_data(timer);";

    rc_send_data = sqlite3_open(logPath, &db_send_data);
    if (rc_send_data != SQLITE_OK)
    {
        makeDatalog("data", "Cannot open send_data database: %s\n", sqlite3_errmsg(db_send_data));
        sqlite3_close(db_send_data);
        return 1;
    }
    rc_send_data = sqlite3_exec(db_send_data, createSql_send_data, 0, 0, &err_send_data);
    if (rc_send_data != SQLITE_OK)
    {
        makeDatalog("data", "SQL error: %s", err_send_data);
        sqlite3_free(err_send_data);
        sqlite3_close(db_send_data);
        return 1;
    }
    makeDatalog("data", "create databases successful!!!");
    return 0;
}

int sendDataInsert(long in_co2, long in_o2, long in_ch4, long in_afm, long out_co2, long out_o2, long out_ch4, long out_afm, int st_temp, int st_humi, char *timer)
{
    sqlite3 *db_send_data;
    sqlite3_stmt *res_send_data;
    int rc_send_data = 0;
    char *err_send_data = 0;
    char *insertSql_send_data = "insert into send_data (m1_co2, m1_o2, m1_ch4, m1_afm, m2_co2, m2_o2, m2_ch4, m2_afm, temp, humi, timer) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    makeDatalog("data", "database insert data : m1_co2=[%d], m1_o2=[%d], m1_ch4=[%d], m1_afm=[%d], m2_co2=[%d], m2_o2=[%d], m2_ch4=[%d], m2_afm=[%d], temp=[%d], humi=[%d], timer=[%s]", in_co2, in_o2, in_ch4, in_afm, out_co2, out_o2, out_ch4, out_afm, st_temp, st_humi, timer);

    // insert send_data
    rc_send_data = sqlite3_open(logPath, &db_send_data);
    if (rc_send_data != SQLITE_OK)
    {
        makeDatalog("data", "Cannot open send_data database: %s\n", sqlite3_errmsg(db_send_data));
        sqlite3_close(db_send_data);
        return 1;
    }
    rc_send_data = sqlite3_prepare_v2(db_send_data, insertSql_send_data, -1, &res_send_data, 0);
    if (rc_send_data == SQLITE_OK)
    {
        sqlite3_bind_int(res_send_data, 1, in_co2);
        sqlite3_bind_int(res_send_data, 2, in_o2);
        sqlite3_bind_int(res_send_data, 3, in_ch4);
        sqlite3_bind_int(res_send_data, 4, in_afm);

        sqlite3_bind_int(res_send_data, 5, out_co2);
        sqlite3_bind_int(res_send_data, 6, out_o2);
        sqlite3_bind_int(res_send_data, 7, out_ch4);
        sqlite3_bind_int(res_send_data, 8, out_afm);

        sqlite3_bind_int(res_send_data, 9, st_temp);
        sqlite3_bind_int(res_send_data, 10, st_humi);

        sqlite3_bind_text(res_send_data, 11, timer, -1, SQLITE_STATIC);
    }
    else
    {
        makeDatalog("data", "Failed to send_data insert execute statement: %s", sqlite3_errmsg(db_send_data));
    }

    sqlite3_step(res_send_data);
    sqlite3_reset(res_send_data);
    sqlite3_free(err_send_data);
    sqlite3_finalize(res_send_data);
    sqlite3_close(db_send_data);

    makeDatalog("data", "send_data inserted!");

    return 0;
}

int sendDataDelete(char *timer)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = 0;
    char *err = 0;

    rc = sqlite3_open(logPath, &db);

    if (rc != SQLITE_OK)
    {
        makeDatalog("data", "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    char *deleteSql = "delete from send_data where timer <= ?;";
    rc = sqlite3_prepare_v2(db, deleteSql, -1, &res, 0);
    if (rc == SQLITE_OK)
    {
        sqlite3_bind_text(res, 1, timer, -1, SQLITE_STATIC);
        makeDatalog("data", "send_data deleted success! [%s]", timer);
    }
    else
    {
        makeDatalog("data", "Failed to delete execute statement: %s", sqlite3_errmsg(db));
    }

    sqlite3_step(res);
    sqlite3_reset(res);
    sqlite3_free(err);
    sqlite3_finalize(res);
    sqlite3_close(db);

    return 0;
}
