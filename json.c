#include "main.h"

/******************************************************************************
 *
 *   함수명
 *       get_json_message()
 *   기능
 *       메시지큐에 전송할 json 형식의 데이터 생성
 *   설명
 *       유입량, 배출량 json 형식으로 데이터 생성함
 *   주의사항
 *       json 형식 {"in_Oxygen":1990,"in_Carbon":80084,"in_Methane":7910,"in_AirCurrent":771,"out_Oxygen":1252,"out_Carbon":71476,"out_Methane":33268,"out_AirCurrent":626,"st_Temp":28,"st_Humi":69}
 *
 ******************************************************************************/
char *get_json_message(long in_co2, long in_o2, long in_ch4, long in_afm, long out_co2, long out_o2, long out_ch4, long out_afm, int st_temp, int st_humi, char *observedDate)
{

    json_object *dataobj = json_object_new_object();

    json_object_object_add(dataobj, "in_Oxygen", json_object_new_int(in_o2));
    json_object_object_add(dataobj, "in_Carbon", json_object_new_int(in_co2));
    json_object_object_add(dataobj, "in_Methane", json_object_new_int(in_ch4));
    json_object_object_add(dataobj, "in_AirCurrent", json_object_new_int(in_afm));
    json_object_object_add(dataobj, "out_Oxygen", json_object_new_int(out_o2));
    json_object_object_add(dataobj, "out_Carbon", json_object_new_int(out_co2));
    json_object_object_add(dataobj, "out_Methane", json_object_new_int(out_ch4));
    json_object_object_add(dataobj, "out_AirCurrent", json_object_new_int(out_afm));
    json_object_object_add(dataobj, "st_Temp", json_object_new_int(st_temp));
    json_object_object_add(dataobj, "st_Humi", json_object_new_int(st_humi));
    json_object_object_add(dataobj, "observedDate", json_object_new_string(observedDate));

    char *jsondata = (char *)json_object_to_json_string(dataobj);

    return jsondata;
}

/******************************************************************************
 *
 *   함수명
 *       get_json_sensor_info()
 *   기능
 *       디비에 저장항 센서 정보를 json 형식으로 변환
 *   설명
 *       유입량, 배출량 json 형식으로 데이터 생성함
 *   주의사항
 *       json 형식 {"in_Oxygen":1990,"in_Carbon":80084,"in_Methane":7910,"in_AirCurrent":771,"out_Oxygen":1252,"out_Carbon":71476,"out_Methane":33268,"out_AirCurrent":626,"st_Temp":28,"st_Humi":69}
 *
 ******************************************************************************/
char *get_json_sensor_info(char *deivceNo, char *deviceId, char *token, long in_co2, long in_o2, long in_ch4, long in_afm, long out_co2, long out_o2, long out_ch4, long out_afm, int st_temp, int st_humi, char *observedDate)
{

    json_object *dataobj = json_object_new_object();

    json_object_object_add(dataobj, "deviceCode", json_object_new_string(deivceNo));
    json_object_object_add(dataobj, "deviceIdnfr", json_object_new_string(deviceId));
    json_object_object_add(dataobj, "accessToken", json_object_new_string(token));
    json_object_object_add(dataobj, "in_Oxygen", json_object_new_int(in_o2));
    json_object_object_add(dataobj, "in_Carbon", json_object_new_int(in_co2));
    json_object_object_add(dataobj, "in_Methane", json_object_new_int(in_ch4));
    json_object_object_add(dataobj, "in_AirCurrent", json_object_new_int(in_afm));
    json_object_object_add(dataobj, "out_Oxygen", json_object_new_int(out_o2));
    json_object_object_add(dataobj, "out_Carbon", json_object_new_int(out_co2));
    json_object_object_add(dataobj, "out_Methane", json_object_new_int(out_ch4));
    json_object_object_add(dataobj, "out_AirCurrent", json_object_new_int(out_afm));
    json_object_object_add(dataobj, "st_Temp", json_object_new_int(st_temp));
    json_object_object_add(dataobj, "st_Humi", json_object_new_int(st_humi));
    json_object_object_add(dataobj, "observedDate", json_object_new_string(observedDate));

    char *jsondata = (char *)json_object_to_json_string(dataobj);

    return jsondata;
}

/******************************************************************************
 *
 *   함수명
 *       get_json_log_info()
 *   기능
 *       디비에 저장항 센서 정보를 json 형식으로 변환
 *   설명
 *       유입량, 배출량 json 형식으로 데이터 생성함
 *   주의사항
 *       json 형식 {"in_Oxygen":1990,"in_Carbon":80084,"in_Methane":7910,"in_AirCurrent":771,"out_Oxygen":1252,"out_Carbon":71476,"out_Methane":33268,"out_AirCurrent":626,"st_Temp":28,"st_Humi":69}
 *
 ******************************************************************************/
char *get_json_log_info(long in_co2, long in_o2, long in_ch4, long in_afm, long out_co2, long out_o2, long out_ch4, long out_afm, int st_temp, int st_humi, char *observedDate)
{

    json_object *dataobj = json_object_new_object();

    json_object_object_add(dataobj, "in_Oxygen", json_object_new_int(in_o2));
    json_object_object_add(dataobj, "in_Carbon", json_object_new_int(in_co2));
    json_object_object_add(dataobj, "in_Methane", json_object_new_int(in_ch4));
    json_object_object_add(dataobj, "in_AirCurrent", json_object_new_int(in_afm));
    json_object_object_add(dataobj, "out_Oxygen", json_object_new_int(out_o2));
    json_object_object_add(dataobj, "out_Carbon", json_object_new_int(out_co2));
    json_object_object_add(dataobj, "out_Methane", json_object_new_int(out_ch4));
    json_object_object_add(dataobj, "out_AirCurrent", json_object_new_int(out_afm));
    json_object_object_add(dataobj, "st_Temp", json_object_new_int(st_temp));
    json_object_object_add(dataobj, "st_Humi", json_object_new_int(st_humi));
    json_object_object_add(dataobj, "observedDate", json_object_new_string(observedDate));

    char *jsondata = (char *)json_object_to_json_string(dataobj);

    return jsondata;
}
