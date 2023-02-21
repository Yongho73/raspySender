#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <json-c/json.h>

char *get_json_message(long in_co2, long in_o2, long in_ch4, long in_afm, long out_co2, long out_o2, long out_ch4, long out_afm, int st_temp, int st_humi, char *observedDate);                                           /* 장치에서 인식한 유입량 및 배출량 메시지큐 데이터 생성 */
char *get_json_sensor_info(char *deviceNo, char *deviceId, char *token, long in_co2, long in_o2, long in_ch4, long in_afm, long out_co2, long out_o2, long out_ch4, long out_afm, int st_temp, int st_humi, char *timer); /* 센서정보 정보 */
char *get_json_log_info(long in_co2, long in_o2, long in_ch4, long in_afm, long out_co2, long out_o2, long out_ch4, long out_afm, int st_temp, int st_humi, char *observedDate);                                          /* 로그용 */
