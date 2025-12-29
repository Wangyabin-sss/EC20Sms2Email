all:
	/home/ubuntu/RK3566tspi/linux/buildroot/output/rockchip_rk3566/host/bin/aarch64-linux-gcc -o smsmail main.c SMS.c utf.c uart.c log.c -I./ -lpthread
