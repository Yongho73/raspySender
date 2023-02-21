#pragma once
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>
#include <stdio.h>

int send_mqserver(char *topic, char *send_message); /* 메시지큐서버로 데이터 전송 */
char *make_topic(char *eqpno);                      /* 메시지큐서버 토픽 생성 */
