#include <stdio.h>                                     //��׼�����������
#include <stdlib.h>                                    //��׼�����ⶨ��
#include <unistd.h>                                    //Unix ��׼��������
#include <sys/types.h>  
#include <sys/stat.h>   
#include <fcntl.h>                                     //�ļ����ƶ���
#include <termios.h>                                   //PPSIX �ն˿��ƶ���
#include <errno.h>                                     //����Ŷ���
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

int ComOpen(const char *port);
int ComSetBaud(int fd, int baud);
int ComInit(const char * dev, int baudrate,int databits,int stopbits,int parity) ;
void ComClose(int fd);
int ComReceive(int fd, char *rbuf, int flag);
int ComSend(int fd, const char *wbuf);