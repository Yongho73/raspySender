gcc -o $1 $1.c mqtt.c curl.c json.c stemp.c log.c prop.c main.c cipher.c sqlite.c restore.c -lwiringPi -lmosquitto -lpthread -lcurl -ljson-c -lssl -lcrypto -lsqlite3
