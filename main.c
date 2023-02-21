#include "main.h"

/******************************************************************************
 *
 *   함수명
 *       main
 *   기능
 *       실행함수
 *   설명
 *       프로세스 실행
 *   주의사항
 *
 ******************************************************************************/
int run(char *timer)
{

    int cnt, result;
    struct termios serial;
    char ch;
    char buffer[BUFFER_SIZE];
    long M1_CO2_Val, M1_O2_Val, M1_AFM_Val, M2_CO2_Val, M2_O2_Val, M2_AFM_Val, ST_TEMP, ST_HUMI;
    long M1_CH4_Val = 0;
    long M2_CH4_Val = 0;
    char *todate = timestemp('T');

    memset(&buffer, 0x00, sizeof(buffer));

    CONFIG conf;
    memset(&conf, 0x00, sizeof(CONFIG));
    setConfig(&conf, CONFIG_PATH);

    int signalGpioPort = atoi(conf.signalGpioPort);

    wiringPiSetupGpio();
    pinMode(signalGpioPort, OUTPUT);

    int fd = open(conf.sensorReadDevice, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        perror(conf.sensorReadDevice);
        return -1;
    }

    if (tcgetattr(fd, &serial) < 0)
    {
        perror("Getting configuration");
        return -1;
    }

    // Set up Serial Configuration
    serial.c_iflag = 0;
    serial.c_oflag = 0;
    serial.c_lflag = 0;
    serial.c_cflag = 0;
    serial.c_cc[VMIN] = 0;
    serial.c_cc[VTIME] = 0;
    serial.c_cflag = B9600 | CS8 | CREAD;

    tcsetattr(fd, TCSANOW, &serial); // Apply configuration

    // 'Turn Off' Request Report Signal
    digitalWrite(signalGpioPort, LOW);
    delay(10);

    // 'Turn On' Request Report Signal
    digitalWrite(signalGpioPort, HIGH);
    delay(50);

    // 'Turn Off' Request Report Signal
    digitalWrite(signalGpioPort, LOW);

    makelog("<read sensor : [%s]>", todate);

    cnt = 0;
    while (1)
    {
        read(fd, &ch, 1);
        buffer[cnt] = ch;
        if (cnt >= READ_SIZE)
        {
            // printf("One Byte Read len=%d\n", cnt);
            makelog("read len : %d", cnt);
            break;
        }
        cnt++;
    }
    buffer[cnt] = '\0';

    makelog("buffer : [%s]", buffer);

    // 00: Sync 'B'
    // 01: Sync 'M'
    // 02: M1 CO2 		: Byte3
    // 03: M1 CO2		: Byte2
    // 04: M1 CO2		: Byte1
    // 05: M1 O2		: Byte3
    // 06: M1 O2		: Byte2
    // 07: M1 O2		: Byte1
    // 08: M1 AirFlow	: High
    // 09: M1 AirFlow	: Low
    // 10: M2 CO2 		: Byte3
    // 11: M2 CO2		: Byte2
    // 12: M2 CO2		: Byte1
    // 13: M2 O2		: Byte3
    // 14: M2 O2		: Byte2
    // 15: M2 O2		: Byte1
    // 16: M2 AirFlow	: High
    // 17: M2 AirFlow	: Low
    // 18: Temperature
    // 19: Humidity
    // 20: CRC8
    // 21: CR (0x0D)

    makelog("received: ");
    for (int i = 0; i < READ_SIZE; i++)
    {
        makelog("%02x ", buffer[i]);
    }
    close(fd);

    // 센서정보 확인 ------------------------------------------------------
    M1_CO2_Val = (buffer[2] << 16) + (buffer[3] << 8) + buffer[4];
    M1_O2_Val = (buffer[5] << 16) + (buffer[6] << 8) + buffer[7];
    M1_AFM_Val = (buffer[8] << 8) + buffer[9];
    M2_CO2_Val = (buffer[10] << 16) + (buffer[11] << 8) + buffer[12];
    M2_O2_Val = (buffer[13] << 16) + (buffer[14] << 8) + buffer[15];
    M2_AFM_Val = (buffer[16] << 8) + buffer[17];
    ST_TEMP = buffer[18];
    ST_HUMI = buffer[19];

    // 테스트 랜덤
    M1_CO2_Val = rand()%1500 + 1200;
    M1_O2_Val = rand()%2000 + 1000;
    M1_AFM_Val = rand()%90 + 50;

    M2_CO2_Val = rand()%1000 + 500;
    M2_O2_Val = rand()%4000 + 3000;
    M2_AFM_Val = rand()%90 + 50;
    
    ST_TEMP = rand()%30 + 20;
    ST_HUMI = rand()%50 + 30;

    // 센서정보 측정시간 ----------------------------------------------------
    char observedDate[15];
    memset(&observedDate, 0x00, sizeof(observedDate));
    strcpy(observedDate, timestemp('H'));
    strcat(observedDate, timer);

    // 전송 데이터 백업 로그 생성 ---------------------------------------------
    char *log_jsonobj;
    if (M1_CO2_Val == 0xBEBEBE && M1_O2_Val == 0xBEBEBE)
    {
        makelog("send data to mqserver default");
        log_jsonobj = get_json_log_info(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, observedDate);
    }
    else
    {
        makelog("send data to mqserver");
        log_jsonobj = get_json_log_info(M1_CO2_Val, M1_O2_Val, M1_CH4_Val, M1_AFM_Val, M2_CO2_Val, M2_O2_Val, M2_CH4_Val, M2_AFM_Val, ST_TEMP, ST_HUMI, observedDate);
    }
    makeBackuplog("send", log_jsonobj);                                                                                                           // 파일백업
    sendDataInsert(M1_CO2_Val, M1_O2_Val, M1_CH4_Val, M1_AFM_Val, M2_CO2_Val, M2_O2_Val, M2_CH4_Val, M2_AFM_Val, ST_TEMP, ST_HUMI, observedDate); // sqlite 입력

    // 장비정보 조회 및 등록 -------------------------------------------------
    DEVICE_INFO device_info;
    memset(&device_info, 0x00, sizeof(DEVICE_INFO));
    int res_get_device_info = register_device(conf.deviceName, conf.deviceNo, conf.deviceGPS, conf.deviceType, conf.exhaustType, &device_info);

    if (res_get_device_info != 2) // 서버 장애
    {
        if (res_get_device_info != 0) // 장비 정보 있음
        {
            // 메시지큐 전송  -----------------------------------------------------
            makelog("<mqserver send>");
            char *topic = make_topic(device_info.device_id);
            char *send_jsonobj;
            if (M1_CO2_Val == 0xBEBEBE && M1_O2_Val == 0xBEBEBE)
            {
                makelog("send data to mqserver default");
                send_jsonobj = get_json_message(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, observedDate);
            }
            else
            {
                makelog("send data to mqserver");
                send_jsonobj = get_json_message(M1_CO2_Val, M1_O2_Val, M1_CH4_Val, M1_AFM_Val, M2_CO2_Val, M2_O2_Val, M2_CH4_Val, M2_AFM_Val, ST_TEMP, ST_HUMI, observedDate);
            }
            result = send_mqserver(topic, send_jsonobj);
            makelog("send mqserver result=[%d]", result);

            // 센서정보 디비화  ----------------------------------------------------
            makelog("<send data to db> [%s]", timer);
            char *sensor_jsonobj = get_json_sensor_info(conf.deviceNo, device_info.device_id, device_info.device_tk, M1_CO2_Val, M1_O2_Val, M1_CH4_Val, M1_AFM_Val, M2_CO2_Val, M2_O2_Val, M2_CH4_Val, M2_AFM_Val, ST_TEMP, ST_HUMI, observedDate);
            makelog("send sensor data plain jsonObj=[%s]", sensor_jsonobj);

            unsigned char sensor_jsonobj_encode[MAXLINE];
            memset(&sensor_jsonobj_encode, 0x00, sizeof(sensor_jsonobj_encode));
            setEncodeStr(sensor_jsonobj, sensor_jsonobj_encode);
            makelog("send sensor data encode jsonObj=[%s]", sensor_jsonobj_encode);

            result = register_sensor_info(sensor_jsonobj_encode);
            makelog("register sensor info result=[%d]", result);
            makelog("\n");

            // 처리결과 업데이트  --------------------------------------------------
            if (result)
            {
                makelog("success!!!");
                renameErrorToReady();
            }
            else
            {
                makelog("register_sensor_info error >>>>>>>>>>");
                makeRestorelog("error", false, log_jsonobj);
            }

            // 메모리 해제  ------------------------------------------------------
            free(topic); /* 메시지큐서버 토픽 */
        }
        else
        {
            makelog("register_device error >>>>>>>>>>");
            makeRestorelog("error", false, log_jsonobj);
        }
    }
    else
    {
        makelog("db access error >>>>>>>>>>");
        makeRestorelog("error", false, log_jsonobj);
    }

    return 0;
}
