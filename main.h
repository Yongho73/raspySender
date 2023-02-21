#pragma once
#include "prop.h"
#include "stemp.h"
#include "log.h"
#include "mqtt.h"
#include "curl.h"
#include "json.h"
#include "cipher.h"
#include "sqlite.h"
#include "restore.h"

#define BUFFER_SIZE 50
#define READ_SIZE 22
#define CONFIG_PATH "/home/pi/CCD_Project_mqtt/config.properties"

int run(char *timer); /* 메인 실행 */
