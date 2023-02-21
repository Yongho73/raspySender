#include "main.h"

/******************************************************************************
 *
 *   함수명
 *       renameErrorToReady
 *   기능
 *       로그파일로 데이터 복원
 *   설명
 *       프로세스 실행
 *   주의사항
 *
 ******************************************************************************/

int renameErrorToReady()
{
    CONFIG conf;
    memset(&conf, 0x00, sizeof(CONFIG));
    setConfig(&conf, CONFIG_PATH);

    struct dirent *de;
    char errorPath[500] = {0};
    char readyPath[500] = {0};
    char *readyDate = timestemp('T');
    sprintf(errorPath, "%s/restore/", conf.logPath);
    sprintf(readyPath, "%s/restore/ready_%s", conf.logPath, readyDate);

    DIR *dr = opendir(errorPath);

    while ((de = readdir(dr)) != NULL)
    {
        if (de->d_type == DT_REG)
        {
            if (strcmp(de->d_name, "error") == 0)
            {
                strcat(errorPath, de->d_name);
                makeDatalog("data", "[%s]->[%s]\n", errorPath, readyPath);
                rename(errorPath, readyPath);
            }
        }
    }

    closedir(dr);
    return 0;
}

int readReady()
{
    CONFIG conf;
    memset(&conf, 0x00, sizeof(CONFIG));
    setConfig(&conf, CONFIG_PATH);

    struct dirent *de = {0};
    char readyPath[500] = {0};
    char readyFile[500] = {0};
    char readyStr[1024] = {0};
    unsigned char sensor_jsonobj_encode[MAXLINE];

    sprintf(readyPath, "%s/restore/", conf.logPath);

    FILE *rfile = NULL;
    DIR *dr = opendir(readyPath);

    char *rfilePStr = {0};
    DEVICE_INFO device_info;
    int res_get_device_info = 0;

    // json 파싱
    json_object *sensor_jsonobj;
    json_object *in_Oxygen_jsonobj;
    json_object *in_Carbon_jsonobj;
    json_object *in_Methane_jsonobj;
    json_object *in_AirCurrent_jsonobj;
    json_object *out_Oxygen_jsonobj;
    json_object *out_Carbon_jsonobj;
    json_object *out_Methane_jsonobj;
    json_object *out_AirCurrent_jsonobj;
    json_object *st_Temp_jsonobj;
    json_object *st_Humi_jsonobj;
    json_object *observedDate_jsonobj;

    // 데이터
    long M1_CO2_Val = 0;
    long M1_O2_Val = 0;
    long M1_CH4_Val = 0;
    long M1_AFM_Val = 0;
    long M2_CO2_Val = 0;
    long M2_O2_Val = 0;
    long M2_CH4_Val = 0;
    long M2_AFM_Val = 0;
    long ST_TEMP = 0;
    long ST_HUMI = 0;
    char *observedDate = {0};

    // 처리 완료
    char complitePath[500] = {0};
    char *nowDate = {0};

    while ((de = readdir(dr)) != NULL)
    {
        if (de->d_type == DT_REG)
        {
            if (strstr(de->d_name, "ready") != NULL)
            {
                memset(&readyFile, 0x00, sizeof(readyFile));
                strcat(readyFile, readyPath);
                strcat(readyFile, de->d_name);

                rfile = NULL;
                rfile = fopen(readyFile, "r");

                if (rfile != NULL)
                {
                    memset(&device_info, 0x00, sizeof(DEVICE_INFO));
                    res_get_device_info = register_device(conf.deviceName, conf.deviceNo, conf.deviceGPS, conf.deviceType, conf.exhaustType, &device_info);
                    
                    if (res_get_device_info == 1)
                    {
                        while (!feof(rfile))
                        {
                            rfilePStr = fgets(readyStr, sizeof(readyStr), rfile);
                            if (rfilePStr != NULL)
                            {
                                // 전송데이터 json 생성
                                sensor_jsonobj = json_tokener_parse(readyStr);
                                // O2 (in)
                                in_Oxygen_jsonobj = json_object_object_get(sensor_jsonobj, "in_Oxygen");
                                M1_O2_Val = json_object_get_int(in_Oxygen_jsonobj);
                                // CO2 (in)
                                in_Carbon_jsonobj = json_object_object_get(sensor_jsonobj, "in_Carbon");
                                M1_CO2_Val = json_object_get_int(in_Carbon_jsonobj);
                                // CH4 (in)
                                in_Methane_jsonobj = json_object_object_get(sensor_jsonobj, "in_Methane");
                                M1_CH4_Val = json_object_get_int(in_Methane_jsonobj);
                                // AFM (in)
                                in_AirCurrent_jsonobj = json_object_object_get(sensor_jsonobj, "in_AirCurrent");
                                M1_AFM_Val = json_object_get_int(in_AirCurrent_jsonobj);
                                // O2 (out)
                                out_Oxygen_jsonobj = json_object_object_get(sensor_jsonobj, "out_Oxygen");
                                M2_O2_Val = json_object_get_int(out_Oxygen_jsonobj);
                                // CO2 (out)
                                out_Carbon_jsonobj = json_object_object_get(sensor_jsonobj, "out_Carbon");
                                M2_CO2_Val = json_object_get_int(out_Carbon_jsonobj);
                                // CH4 (out)
                                out_Methane_jsonobj = json_object_object_get(sensor_jsonobj, "out_Methane");
                                M2_CH4_Val = json_object_get_int(out_Methane_jsonobj);
                                // AFM (out)
                                out_AirCurrent_jsonobj = json_object_object_get(sensor_jsonobj, "out_AirCurrent");
                                M2_AFM_Val = json_object_get_int(out_AirCurrent_jsonobj);
                                // TEMP
                                st_Temp_jsonobj = json_object_object_get(sensor_jsonobj, "st_Temp");
                                ST_TEMP = json_object_get_int(st_Temp_jsonobj);
                                // AFM
                                st_Humi_jsonobj = json_object_object_get(sensor_jsonobj, "st_Humi");
                                ST_HUMI = json_object_get_int(st_Humi_jsonobj);
                                // observedDate
                                memset(&observedDate, 0x00, sizeof(observedDate));
                                observedDate_jsonobj = json_object_object_get(sensor_jsonobj, "observedDate");
                                observedDate = (char *)json_object_get_string(observedDate_jsonobj);

                                // json string convert
                                char *sensor_jsonobj = get_json_sensor_info(conf.deviceNo, device_info.device_id, device_info.device_tk, M1_CO2_Val, M1_O2_Val, M1_CH4_Val, M1_AFM_Val, M2_CO2_Val, M2_O2_Val, M2_CH4_Val, M2_AFM_Val, ST_TEMP, ST_HUMI, observedDate);
                                makeDatalog("data", "send sensor data plain jsonObj=[%s]", sensor_jsonobj);
                                memset(&sensor_jsonobj_encode, 0x00, sizeof(sensor_jsonobj_encode));
                                setEncodeStr(sensor_jsonobj, sensor_jsonobj_encode);
                                makeDatalog("data", "send sensor data encode jsonObj=[%s]\n", sensor_jsonobj_encode);

                                int result = register_sensor_info(sensor_jsonobj_encode);
                                makeDatalog("data", "register sensor info result=[%d]\n", result);

                                // 전송 실패시 데이터 로그 생성
                                if (result)
                                {
                                    makeDatalog("data", "...success...\n");
                                }
                                else
                                {
                                    makeDatalog("data", "...fail...\n");
                                    exit(0);
                                }
                                sleep(1); // 1초간격
                            }
                        }
                        fclose(rfile);
                        // 재처리 방지를 위해 처리완료 rename
                        nowDate = timestemp('T');
                        memset(&complitePath, 0x00, sizeof(complitePath));
                        sprintf(complitePath, "%s/restore/done_%s", conf.logPath, nowDate);
                        rename(readyFile, complitePath);
                        makeDatalog("data", "done=[%s]->[%s]\n", readyFile, complitePath);
                    }
                }
            }
        }
    }
    closedir(dr);
    return 0;
}
