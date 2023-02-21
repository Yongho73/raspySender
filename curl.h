#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <json-c/json.h>

#define API_URI_DEV_REGI "/deviceRegi"
#define API_URI_INF_REGI "/ccd/observedRegi"

typedef struct
{
    char *response;
    size_t size;
} CURL_RES_DATA;

typedef struct
{
    char device_id[50];
    char device_tk[100];
} DEVICE_INFO;

void init_curl_response_data(CURL_RES_DATA *res);                                                                 /* curl 옵션 */
size_t curl_write_function(void *ptr, size_t size, size_t nmemb, CURL_RES_DATA *res);                             /* curl 옵션 */
char *call_api_url(char *type, char *api_uri, char *post_data);                                                   /* rest api 호출 */
int register_device(char *dev_nm, char *dev_no, char *dev_gp, char *dev_dt, char *dev_et, DEVICE_INFO *dev_info); /* 장비정보 등록 */
int register_sensor_info(char *sensor_data);                                                                      /* 센서정보 디비화 */
