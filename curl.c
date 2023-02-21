#include "main.h"

CURL *curl;
CURLcode res;
CURL_RES_DATA res_data;

void init_curl_response_data(CURL_RES_DATA *res)
{

    res->size = 0;
    res->response = (char *)malloc(res->size + 1);
    if (res->response == NULL)
    {
        makelog("init_curl_response_data malloc() failed\n");
    }
    res->response[0] = '\0';
}

size_t curl_write_function(void *ptr, size_t size, size_t nmemb, CURL_RES_DATA *res)
{
    size_t index = res->size;
    size_t n = (size * nmemb);
    char *tmp;
    res->size += (size * nmemb);
    tmp = (char *)realloc(res->response, res->size + 1); /* +1 for '\0' */
    if (tmp)
    {
        res->response = tmp;
    }
    else
    {
        if (res->response)
        {
            free(res->response);
        }
        makelog("Failed to allocate memory.\n");
        return 0;
    }
    memcpy((res->response + index), ptr, n);
    res->response[res->size] = '\0';
    return size * nmemb;
}

/******************************************************************************
 *
 *   함수명
 *       call_api_url()
 *   기능
 *       rest api 호출 (http 통신)
 *   설명
 *       서버 api 를 호출하고 결과값을 리턴
 *   주의사항
 *
 ******************************************************************************/
char *call_api_url(char *type, char *api_uri, char *post_data)
{
    CONFIG conf;
    memset(&conf, 0x00, sizeof(CONFIG));
    setConfig(&conf, CONFIG_PATH);

    char host[60];
    memset(&host, 0x00, sizeof(host));
    sprintf(host, "%s:%s", conf.apiHost, conf.apiPort);

    int len_api_url = strlen(host) + strlen(api_uri);
    int len_pot_dat = strlen(post_data) + 1; // json 라이브러리를 이용하면 1byte 가 필요하다

    char api_url[len_api_url];
    char api_pam[len_pot_dat];

    memset(&api_url, 0x00, sizeof(api_url));
    memset(&api_pam, 0x00, sizeof(api_pam));

    strcat(api_url, host);
    strcat(api_url, api_uri);

    makelog("api url : [%s]", api_url);

    if (len_pot_dat != 0)
    {
        strcat(api_pam, post_data);
        makelog("api dat : [%s]", api_pam);
    }

    
    struct curl_slist *headers = NULL;

    init_curl_response_data(&res_data);

    curl = curl_easy_init();

    if (curl)
    {
        if (strcmp(type, "text") == 0)
        {
            headers = curl_slist_append(headers, "Accept: text/plain");
            headers = curl_slist_append(headers, "Content-Type: text/plain");
        }
        else
        {
            headers = curl_slist_append(headers, "Accept: application/json");
            headers = curl_slist_append(headers, "Content-Type: application/json");
        }

        headers = curl_slist_append(headers, "charsets: utf-8");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, api_url);

        if (len_pot_dat != 0)
        {
            // curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, api_pam);
        }

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res_data);
        
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L); // api 응답 대기 시간 (3초 설정)
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L); // sigalrm 발생 방지

        //curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT,  3);
        //curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1);
        //curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 15);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            makelog("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }

    return res_data.response;
}

/******************************************************************************
 *
 *   함수명
 *       register_device(장치명, 장치번호, 장비위치, 장비정보(리턴))
 *   기능
 *       rest api 호출에 필요한 token 추출
 *   설명
 *       서버 api 를 호출하고 json 결과값 내에 accessToken 키값을 리턴함
 *   주의사항
 *
 ******************************************************************************/
int register_device(char *dev_nm, char *dev_no, char *dev_gp, char *dev_dt, char *dev_et, DEVICE_INFO *dev_info)
{

    json_object *dataobj = json_object_new_object();
    json_object_object_add(dataobj, "deviceName", json_object_new_string(dev_nm));
    json_object_object_add(dataobj, "deviceCode", json_object_new_string(dev_no));
    json_object_object_add(dataobj, "location_DD", json_object_new_string(dev_gp));
    json_object_object_add(dataobj, "deviceType", json_object_new_string(dev_dt));
    json_object_object_add(dataobj, "exhaustType", json_object_new_string(dev_et));

    char *pa_data = (char *)json_object_to_json_string(dataobj);

    char *response = call_api_url("json", API_URI_DEV_REGI, pa_data);

    // printf("res json string : [%s]\n", response);

    int len = strlen(response) + 1;
    char res_json_str[len];
    memset(&res_json_str, 0x00, sizeof(res_json_str));
    memcpy(res_json_str, response, sizeof(res_json_str));

    // printf("res json string : [%s]\n", res_json_str);

    json_object *resObj = json_tokener_parse(res_json_str);
    json_object *success = json_object_object_get(resObj, "success");
    json_object *data = json_object_object_get(resObj, "data");
    char *result = (char *)json_object_get_string(success);

    makelog("\n==>register_device response result=[%s]<==\n", result);

    if (result != NULL)
    {
        if (strcmp(result, "true") == 0)
        {
            json_object *deviceIdnfr = json_object_object_get(data, "deviceIdnfr");
            json_object *accessToken = json_object_object_get(data, "accessToken");
            char *devid = (char *)json_object_get_string(deviceIdnfr);
            char *token = (char *)json_object_get_string(accessToken);

            memcpy(dev_info->device_id, devid, sizeof(dev_info->device_id));
            memcpy(dev_info->device_tk, token, sizeof(dev_info->device_tk));

            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

/******************************************************************************
 *
 *   함수명
 *       register_sensor_info(센서정보)
 *   기능
 *       센서정보 저장 api 에 센서정보를 전송함
 *   설명
 *       센서정보 저장 api 에 센서정보를 전송함
 *   주의사항
 *
 ******************************************************************************/
int register_sensor_info(char *sensor_data)
{

    int len = strlen(sensor_data) + 1;
    char res_json_str[len];
    memset(&res_json_str, 0x00, sizeof(res_json_str));
    memcpy(res_json_str, sensor_data, sizeof(res_json_str));

    char *response = call_api_url("json", API_URI_INF_REGI, res_json_str);

    json_object *resObj = json_tokener_parse(response);
    json_object *success = json_object_object_get(resObj, "success");
    char *result = (char *)json_object_get_string(success);

    makelog("\n==>register_sensor_info response result=[%s]<==\n", result);

    if (result != NULL)
    {
        if (strcmp(result, "true") == 0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
