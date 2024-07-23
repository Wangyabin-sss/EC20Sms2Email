#!/bin/sh

cd /root
export TZ=CST-8
export EMAILKEYVAL=email密钥
ttydev="$(echo `ls /dev/ttyUSB*` | awk '{print $3}')"

while true
do
	if [ `ps -ef | grep smsmail | grep -v "grep" | wc -l` == "0" ]
	then
		./smsmail $ttydev &
	fi

	if [ "$ttydev" != "$(echo `ls /dev/ttyUSB*` | awk '{print $3}')" ]
    then
        ttydev="$(echo `ls /dev/ttyUSB*` | awk '{print $3}')"
		killall -9 smsmail
    fi

	sleep 10
done

