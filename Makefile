all:
	/home/ubuntu/Desktop/rkcpu/rk3566-tspi/buildroot/output/rockchip_rk3566/host/bin/aarch64-linux-gcc -o smsmail main.c SMS.c utf.c uart.c log.c -I./ -lpthread
