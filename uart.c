#include "uart.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <errno.h>

/*******************************************************************************
 *
 ******************************************************************************/
int get_termios(int fd, struct termios *s)
{
	if( -1 == fd || 0 == s )
	{
		return -1;
	}
	return tcgetattr(fd, s);
}

/*******************************************************************************
 *
 ******************************************************************************/
int set_termios(int fd, const struct termios *s)
{
	if( -1 == fd || 0 == s )
	{
		return -1;
	}
	return tcsetattr(fd, TCSANOW, s);
}

/*******************************************************************************
 *
 ******************************************************************************/
int baud_to_speed(int baud)
{
	switch( baud )
	{
		case B1200:
			return 1200;
		case B2400:
			return 2400;
		case B4800:
			return 4800;
		case B9600:
			return 9600;
		case B19200:
			return 19200;
		case B38400:
			return 38400;
		case B57600:
			return 57600;
		case B115200:
			return 115200;
	}
	return 0;
}

/*******************************************************************************
 *
 ******************************************************************************/
int get_speed(const struct termios *s)
{
	if( s )
	{
		return baud_to_speed(s->c_cflag & CBAUD);
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
int get_ispeed(const struct termios *s)
{
	if( s )
	{
		return baud_to_speed(s->c_iflag & CBAUD);
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
int get_ospeed(const struct termios *s)
{
	if( s )
	{
		return baud_to_speed(s->c_oflag & CBAUD);
	}
	return 0;
}

/*******************************************************************************
 *
 ******************************************************************************/
int get_bsize(const struct termios *s)
{
	if( s )
	{
		switch(s->c_cflag & CSIZE)
		{
			case CS5:
				return 5;
			case CS6:
				return 6;
			case CS7:
				return 7;
			case CS8:
				return 8;
		}
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
char get_parity(const struct termios *s)
{
	if( s )
	{
		if( s->c_cflag & PARENB )
		{
			if( s->c_cflag & PARODD )
			{
				return 'O';
			}
			else
			{
				return 'E';
			}
		}
		return 'N';
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
int get_stop(const struct termios *s)
{
	if( s )
	{
		if( s->c_cflag & CSTOPB )
		{
			return 2;
		}
		return 1;
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
int speed_to_baud(int speed)
{
	switch(speed)
	{
		case 1200:
			return B1200;
		case 2400:
			return B2400;
		case 4800:
			return B4800;
		case 9600:
			return B9600;
		case 19200:
			return B19200;
		case 38400:
			return B38400;
		case 57600:
			return B57600;
		case 115200:
			return B115200;
	}
	return B9600;
}

/*******************************************************************************
 *
 ******************************************************************************/
int set_speed(struct termios *s, int speed)
{
	if( s )
	{
		s->c_cflag &= ~CBAUD;
		s->c_cflag |= speed_to_baud(speed);
		return 0;
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
int set_ispeed(struct termios *s, int speed)
{
	if( s )
	{
		return cfsetispeed(s, speed_to_baud(speed) );
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
int set_ospeed(struct termios *s, int speed)
{
	if( s )
	{
		return cfsetospeed(s, speed_to_baud(speed) );
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
int set_bsize(struct termios *s, int bsize)
{
	if( s )
	{
		s->c_cflag &= ~CSIZE;
		switch( bsize )
		{
			case 5:
				s->c_cflag |= CS5;
				break;
			case 6:
				s->c_cflag |= CS6;
				break;
			case 7:
				s->c_cflag |= CS7;
				break;
			case 8:
				s->c_cflag |= CS8;
				break;
		}
		return 0;
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
int set_parity(struct termios *s, char parity)
{
	if( s )
	{
		switch(parity)
		{
			case 'n':
			case 'N':
				s->c_cflag &= ~PARENB;
				break;
			case 'o':
			case 'O':
				s->c_cflag |= PARENB;
				s->c_cflag |= PARODD;
				break;
			case 'e':
			case 'E':
				s->c_cflag |= PARENB;
				s->c_cflag &= ~PARODD;
				break;
		}
		return 0;
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
int set_stop(struct termios *s, int stop)
{
	if( s )
	{
		if( 1 == stop )
		{
			s->c_cflag &= ~CSTOPB;
		}
		else
		{
			s->c_cflag |= CSTOPB;
		}
		return 0;
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
int enable_read(struct termios *s)
{
	if( s )
	{
		// s->c_cflag |= CREAD;
		s->c_cflag |= (CREAD | CLOCAL);
		return 0;
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
int disable_read(struct termios *s)
{
	if( s )
	{
		s->c_cflag &= ~CREAD;
		return 0;
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
int enable_flow_control(struct termios *s)
{
	if( s )
	{
		s->c_cflag |= CRTSCTS;
		return 0;
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
int disable_flow_control(struct termios *s)
{
	if( s )
	{
		s->c_cflag &= ~CRTSCTS;
		return 0;
	}
	return -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
void show_termios(const struct termios *s)
{
	if( s )
	{
		printf("speed=%d\n", get_speed(s));
		printf("parity=%c\n", get_parity(s));
		printf("bsize=%d\n", get_bsize(s));
		printf("stop=%d\n", get_stop(s));
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
int uart_config(int fd, int baud, char parity, int bsize, int stop)
{
	if( fd < 0 )
	{
		return -1;
	}
	else
	{
		struct termios newDev = {0};

		bzero(&newDev, sizeof(newDev));
		if( set_speed(&newDev, baud) )
		{
			return -2;
		}
		if( set_parity(&newDev, parity) )
		{
			return -3;
		}
		if( set_bsize(&newDev, bsize) )
		{
			return -4;
		}
		if( set_stop(&newDev, stop) )
		{
			return -5;
		}
		enable_read(&newDev);
		disable_flow_control(&newDev);

		/* Raw input */
		newDev.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

		/* Software flow control is disabled */
		newDev.c_iflag &= ~(IXON | IXOFF | IXANY);

		/* Raw output */
		newDev.c_oflag &=~ OPOST;

		/* Unused because we use open with the NDELAY option */
		newDev.c_cc[VMIN] = 0;
		newDev.c_cc[VTIME] = 0;

		if( set_termios(fd, &newDev) )
		{
			return -6;
		}
	}
	return 0;
}

int uart_open(const char *dev, int baud, char parity, int data_bit, int stop_bit)
{
    int fd = open(dev, O_RDWR|O_NOCTTY|O_NDELAY|O_EXCL|O_CLOEXEC);
	// int fd = open(dev, O_RDWR|O_NOCTTY|O_NDELAY);
    if(fd == -1)
    {
        printf("open, %s\n", strerror(errno));
        return -1;
    }

    int retval = uart_config(fd, baud, parity, data_bit, stop_bit);
    if(retval != 0)
    {
        printf("uart_config, error_code = %d\n", retval);
        return -1;
    }

    tcflush(fd, TCIOFLUSH);

    printf("%s, %d/%d/%d/%c\n", dev, baud, data_bit, stop_bit, parity);

    return fd;
}

int uart_close(int fd)
{
    return close(fd);
}

int uart_send(int fd, const unsigned char *data, unsigned int data_len)
{
    int send_num = write(fd, data, data_len);
    if(send_num == -1)
    {
        printf("write, %s\n", strerror(errno));
        return -1;
    }
    else if(send_num != data_len)
    {
        printf("send_num != data_len\n");
        return -1;
    }
    return send_num;
}

int uart_recv(int fd, unsigned char *buf, unsigned int buf_size)
{
    int recv_num = read(fd, buf, buf_size);
    if(recv_num == -1)
    {
        printf("read, %s\n", strerror(errno));
        return -1;
    }
    return recv_num;
}

int uart_recv_within_timeout(int fd, unsigned char *buf, unsigned int buf_size, const struct timeval *timeout)
{
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

	int retval = -1;

	if(timeout)
	{
		struct timeval s_timeout = *timeout;
		retval = select(fd + 1, &rfds, NULL, NULL, &s_timeout);
	}
	else
	{
		retval = select(fd + 1, &rfds, NULL, NULL, NULL);
	}

    if(retval == -1)
    {
        printf("select, %s\n", strerror(errno));
        return -1;
    }
    else if(retval == 0)
    {
        // printf("select tiemout\n");
        return -2;
    }

    int recv_num = read(fd, buf, buf_size);
    if(recv_num == -1)
    {
        printf("read, %s\n", strerror(errno));
        return -1;
    }

    return recv_num;
}
