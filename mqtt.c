#include "main.h"

/******************************************************************************
 *
 *   함수명
 *       send_mqserver(토픽, 메시지)
 *   기능
 *       메시지큐서버로 데이터 전송
 *   설명
 *       메시지큐서버로 데이터 전송
 *   주의사항
 *
 ******************************************************************************/
int send_mqserver(char *topic, char *send_message)
{

    CONFIG conf;
    memset(&conf, 0x00, sizeof(CONFIG));
    setConfig(&conf, CONFIG_PATH);

    struct mosquitto *mosq = NULL;

    // 초기화
    mosquitto_lib_init();

    // 모스키토 런타임 객체와 클라이언트 랜덤 ID 생성
    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq)
    {
        makelog("Cant initiallize mosquitto library");
        return 0;
    }

    mosquitto_username_pw_set(mosq, conf.mqttUser, conf.mqttPswd);

    // MQTT 서버 연결 설립, keep-alive 메시지 사용 안함
    int ret = mosquitto_connect(mosq, conf.mqttHostIp, atoi(conf.mqttPort), 0);
    if (ret)
    {
        makelog("Cant connect to mosquitto server");
        return 0;
    }

    makelog("send topic : [%s][%s][%s]=[%s]", conf.mqttHostIp, conf.mqttPort, topic, send_message);

    ret = mosquitto_publish(mosq, NULL, topic, strlen(send_message), send_message, 0, false);
    if (ret)
    {
        makelog("Cant connect to mosquitto server");
        return 0;
    }

    // 네트워크 동작이 끝나기 전에 모스키토 동작을 막기위해 잠깐의 딜레이가 필요함
    // sleep(1);

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 1;
}

/******************************************************************************
 *
 *   함수명
 *       make_topic(장치번호)
 *   기능
 *       메시지큐서버 토픽 데이터 생성
 *   설명
 *       메시지큐서버 토픽 데이터 생성
 *   주의사항
 *       형식 : ccdm/장치명/data
 *
 ******************************************************************************/
char *make_topic(char *eqpno)
{
    char *topic = (char *)malloc(sizeof(char) * 20);
    topic[0] = '\0';
    sprintf(topic, "ccdm/%s/data", eqpno);
    return topic;
}
