#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SMS.h"
#include <locale.h>
#include "utf.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "uart.h"
#include <time.h>
#include "log.h"

#define printf Dbuginfo
#define SMSMAXNUMS      35      //达到这个数据量就转储到文件，从ec20中删除信息
#define RECVBUFFERSIZE  1024    //串口接收数据缓存区大小
#define MAILTXT "/root/mail.txt"
#define SMSHISTORYTXT "/root/smsHistory.txt"
#define DSC_to_msg(DSC) (DSC == 0 ? "Bit7" : (DSC == 1 ? "Bit8" : "UCS2"))
char *mailmsghead = "From: 15217681799 <w15217681799@163.com>\nTo: 远行 <3253941815@qq.com>\nSubject: sms to mail\n";  //邮件格式头
char *KeyVal;
char *curlcmd = "curl --verbose --ssl-reqd --url \"smtp://smtp.163.com\" --mail-from \"w15217681799@163.com\" --mail-rcpt \"3253941815@qq.com\" --upload-file /root/mail.txt --cacert \"/root/cacert.pem\" --user \"w15217681799@163.com:";


void write_sms_txt(struct SMS_Struct *sms)
{
    char data[1024]={0};
    char date[64];
    sprintf(data, "服务中心地址: %s\n发送方地址: %s\n服务中心时间戳: %s\n消息内容: %s\n数据编码方案： %s\n消息类型： %d\n", sms->SCA,
                                                                                                                    sms->OA,
                                                                                                                    sms->SCTS,
                                                                                                                    sms->UD,
                                                                                                                    DSC_to_msg(sms->DCS),
                                                                                                                    sms->MC);
    printf("sms msg:\n%s\n",data);
    time_t now = time(NULL); // 获取当前时间
    struct tm *t = localtime(&now); // 转换为本地时间

    sprintf(date,"Date: %d %d %d  %d:%d:%d\n\n",
           t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
           t->tm_hour, t->tm_min, t->tm_sec);
    FILE *file = fopen(MAILTXT,"w");
    fwrite(mailmsghead,1,strlen(mailmsghead),file);
    fwrite(date,1,strlen(date),file);
    fwrite(data,1,strlen(data),file);
    fclose(file);
}

void free_sms_data(struct SMS_Struct *sms)
{
    // char *SCA;         // 服务中心地址
    // char *OA;          // 发送方地址
    // char *SCTS;        // 服务中心时间戳
    // struct UDHS *UDH;     // 用户数据头
    // char *UD;          // 用户数据
    free(sms->SCA);
    free(sms->OA);
    free(sms->SCTS);
    free(sms->UD);
    if(sms->UDH!=NULL)
    {
        free(sms->UDH->UDH->IED);
        free(sms->UDH->UDH);
        free(sms->UDH);
    }
}

void send_mail_curl()
{
    char popencmd[512];
    char buf[1024];
    strcpy(popencmd,curlcmd);
    strcat(popencmd,KeyVal);
    strcat(popencmd,"\"");
    printf("cmd:%s\n",popencmd);
    FILE* p_file = popen(popencmd,"r");
    if (!p_file) {  
        printf("Error to popen\n");  
    }  
  
    while (fgets(buf, 1024, p_file) != NULL) {
        printf("%s", buf);
    }  
    pclose(p_file);
}

// char* get_AT_ttyUSB(void)
// {
//     int ret;
//     static char buf[32];
//     buf[0] = 0;
//     FILE *getttycmd = popen("echo `ls /dev/ttyUSB*` | awk '{print $1}'","r");
//     ret = fread(buf,1,32,getttycmd);
//     pclose(getttycmd);
//     if(ret<4)
//         return NULL;
//     else
//         return buf;
// }

int main(int argc, char *argv[])
{
    Dbuginit("/root/log.txt");
    printf("App start\n");

    char *ttydev;
    // ttydev = get_AT_ttyUSB();
    // if(ttydev==NULL)
    // {
    //     printf("AT tty dev not find \n");
    //     return -1;
    // }
    // printf("AT TTY is = %s strlen = %d\n",ttydev,strlen(ttydev));
    
    if(argc < 2)
    {
        printf("you maybe use args ./smsmail /dev/ttyUSB2\n");
        return -1;
    }
    ttydev = argv[1];
    printf("AT TTY is = %s strlen = %d\n",ttydev,strlen(ttydev));


     // 获取环境变量的值
    KeyVal = getenv("EMAILKEYVAL");
    if(KeyVal==NULL)
    {
        printf("env EMAILKEYVAL not found\n");
        return -1;
    }
    int uartfd = uart_open(ttydev,115200,'N',8,1);
    int ret;
    char recvbuf[RECVBUFFERSIZE],pbuf[RECVBUFFERSIZE];
    char *p;
    struct SMS_Struct sms;
    int smsnumid=0;

    while(1)
    {
		memset(recvbuf,0,RECVBUFFERSIZE);
        ret = uart_recv(uartfd, recvbuf, RECVBUFFERSIZE);
        if(ret==-1)
        {
            printf("recv data error:%s\n",strerror(errno));
            break;
        }
        else if(ret>0)
        {
            printf("ret = %d recv data success:\n%s\n",ret ,recvbuf);
            if(strstr(recvbuf,"+CMTI:")!=NULL)
            {
                p = strstr(recvbuf,",");
                smsnumid = atoi(p+1);
                sprintf(pbuf,"AT+CMGR=%d\r\n",smsnumid);
                printf("send cmd=%s\n",pbuf);
                uart_send(uartfd, pbuf, strlen(pbuf));
            }
            else if((p = strstr(recvbuf,"+CMGR:"))!=NULL)
            {
                p = strstr(p,"\n");
                strcpy(pbuf,p+1);
				p = strstr(pbuf,"\r\n");
				if(p!=NULL)
                	*p=0;
                printf("pdu msg:%s\n",pbuf);
                sms = PDUDecoding(pbuf);
                write_sms_txt(&sms);
                send_mail_curl();
                free_sms_data(&sms);

                if(smsnumid >= SMSMAXNUMS)
                {
                    sprintf(pbuf,"AT+CMGL=4\r\n");
                    printf("send cmd=%s\n",pbuf);
                    uart_send(uartfd, pbuf, strlen(pbuf));
                }
            }
            else if(strstr(recvbuf,"+CMGL:")!=NULL)
            {
                FILE *file = fopen(SMSHISTORYTXT,"a");
                fwrite(recvbuf,1,ret,file);
                fclose(file);
                if(strstr(recvbuf,"OK\r\n")!=NULL)
                {
                    sprintf(pbuf,"AT+CMGD=1,4\r\n");
                    printf("send cmd=%s\n",pbuf);
                    uart_send(uartfd, pbuf, strlen(pbuf));
                }
            }
			if(ret == RECVBUFFERSIZE)
				continue;
        }
        sleep(3);
    }

    uart_close(uartfd);
    return 0;
}

/*
int main() {
    char *out;
    int len;
    out = (char*)malloc(1024);

    //struct SMS_Struct s = PDUDecoding("0891683108502905F0040D91683159298373F70008512111013253232A00610062006300640065006600670040002400250022002A002D98769876987698769876987698769876");
    //struct SMS_Struct s = PDUDecoding("0891683108502905F0040D91683159298373F70000512101616500232261F1985C369F01234149A46ABD50A990E8A4DBB17E31D9AC56B3DD703918");
    //struct SMS_Struct s = PDUDecoding("0891683108801505F94405A10180F60008512111901592238C0500035F02014E3A5E86795D8D355DDE79FB52A800340047002B8BD555467528FF0C6211516C53F8672C6708541160A88D60900177015185514D8D39901A8BDD65F6957F00310030003000305206949FFF084EF7503C0032003000305143FF09FF0C4E0E60A851714EAB7CBE5F6979FB52A800340047751F6D3BFF016CE8FF1A8D60900165F6957F4EC59650");
    //SMS_Struct s = sms->PDUDecoding("0011000D91683118307447F50000A70100");
    //struct SMS_Struct s = PDUDecoding("0891683108502905F0400D91683159298373F70008512141615053238C0500031502015C0A656C76845BA26237FF0C60A8597DFF0160A84E8E00320030003100355E740031003267080031003165E50031003870B900350038520670B964AD621663A565364E864E2D56FD79FB52A87684632F94C30032002E0030003051434E1A52A1FF0C8D448D390032002E00300030514330025982679C60A890475230672A653652305DF270B9");
    struct SMS_Struct s = PDUDecoding(
           "0891683108502905F0440D91683159298373F70008512141615093238005000315020264AD4FE1606F3001653652304FE1606F4E0E5BA34F204E0D7B267B4995EE989862165BF9670D52A14E0D6EE1610FFF0C53EF57280031003267080031003165E50032003270B9003300355206524D56DE590D0032003500370033FF0C62114EEC5C064E0D653653D660A876848D39752830024E2D56FD79FB52A8");
    //SMS_Struct s = sms->PDUDecoding("0891683110602305F0040D91685175605321F40000512161716000230761F1985C369F01");
    //SMS_Struct s = sms->PDUDecoding("0891683110602305F0040D91685175605321F40008512161716065230C4E2D56FD4EBA4E2D56FD4EBA");

    printf("服务中心地址: %s\n", s.SCA);
    printf("发送方地址: %s\n", s.OA);
    printf("服务中心时间戳: %s\n", s.SCTS);
    printf("消息内容: %s\n", s.UD);
    printf("数据编码方案： %s\n", DSC_to_msg(s.DCS));
    printf("消息类型： %d\n", s.MC);

    printf("\n测试编码\n");
    char *msg = "你是谁？test hello!哈#@我看将了的罚款拉大方急啊士大夫阿达发射的将发卡十大经典分开了阿斯顿附件阿斯顿看了几发达附件阿斯顿颗粒的发达反倒是方阿斯顿方阿达方阿斯顿发速度噶速度高飞的该";
    //char *msg = "adhfasjj3r34-534=53450323#@*()$*_)jkjf*@#$__#++U_@*)(U_)UJ83239239u35j02=1-189hj1=111834449^^%%#&*&#*((()&kfjdafjdasfoH*ty8&*T6:7G&6Rg&*t6oG&F^&F(^&ROG&*^&GOG7$%^&*_#$IFG^&TF90&&^@)@_+)*(^&++8娃娃";
    //char *msg = "这是从rild发出的测试短信!";
    //char *msg = "abcdefg?@#$%^&*)(*_";

    struct PDUS *pdus = PDUEncoding("+8613811420981", msg, NULL);
    printf("长短信编码：\n");

    for (int i = 0; i < pdus->count; i++) {
        printf("第 %d 条:\n", i + 1);
        printf("%s\n", pdus->PDU[i]);
    }

    return 0;
}
*/
