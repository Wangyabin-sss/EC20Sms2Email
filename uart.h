#ifndef __UART_H__
#define __UART_H__

#include <sys/select.h>

int uart_open(const char *dev, int baud, char parity, int data_bit, int stop_bit);
int uart_close(int fd);

int uart_send(int fd, const unsigned char *data, unsigned int data_len);
int uart_recv(int fd, unsigned char *buf, unsigned int buf_size);
int uart_recv_within_timeout(int fd, unsigned char *buf, unsigned int buf_size, const struct timeval *timeout);

#endif