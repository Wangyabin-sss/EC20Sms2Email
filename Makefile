all:
	arm-linux-gnueabihf-gcc -o smsmail main.c SMS.c utf.c uart.c log.c -I./ -lpthread
