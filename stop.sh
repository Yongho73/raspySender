#!/bin/sh
PID=`ps -ef|grep ccdm|grep -v "grep"|awk '{print $2}'`
#PID=`ps ax|grep ccdm|grep -v "grep"|grep -v $O|awk '{print $2}'`
if [ -n "$PID" ]; then
    echo "=====ccdm is running at" $PID "Shutdown ccdm now"
    kill -9 $PID
else
    echo "=====ccdm isn't running====="
fi
