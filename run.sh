#!/bin/sh

cd /root
export EMAILKEYVAL=email授权码
while true
do
./smsmail /dev/ttyUSB2
sleep 5
done

