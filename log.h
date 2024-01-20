#ifndef _LOG_H
#define _LOG_H

#define LOGCMD          0x0
#define LOGFILE         0x1
#define LOGNULL         0x2
#define LOGMODEL        LOGFILE

#define DBUG_BUF_LINE   0x10    //行缓冲
#define DBUG_BUF_ALL    0x11    //全缓冲
#define DBUG_BUF_MODEL  DBUG_BUF_LINE
#define DBUG_BUF_SIZE   1024    //缓冲区大小

#define LOGFILESIZE     3*1024*1024
#define LOG_WITHTIME    1

#include <stdio.h>

/*

*/
void log_init(char *filename);
void log_2_cmd(int model, char *fmt, ...);
void log_2_file(int model, char *fmt, ...);
char *time_now(void);


#if (LOGMODEL==LOGCMD)
#define Dbuginit()  do{log_init("");}while(0)
#define Dbuginfo(fmt , ...)   do{log_2_cmd(0, fmt , ##__VA_ARGS__);}while(0)
#define Dbugerror(fmt , ...)   do{log_2_cmd(1, fmt , ##__VA_ARGS__);}while(0)
#define Dbugwarning(fmt , ...)   do{log_2_cmd(2, fmt , ##__VA_ARGS__);}while(0)
#else
#define Dbuginit(file)  do{log_init(file);}while(0)
#define Dbuginfo(fmt , ...)   do{log_2_file(0, fmt , ##__VA_ARGS__);}while(0)
#define Dbugerror(fmt , ...)   do{log_2_file(1, fmt , ##__VA_ARGS__);}while(0)
#define Dbugwarning(fmt , ...)   do{log_2_file(2, fmt , ##__VA_ARGS__);}while(0)
#endif



#endif

