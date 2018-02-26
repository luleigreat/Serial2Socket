#include "comdev.h"
#include "config.h"	
#include <pthread.h>

#define MAXLINE 4096
#define MAXDATASIZE 1024    //缓冲区大小  
#define BACKLOG 50           //listen队列等待的连接数  

int sockfdClient = 0;
int sockfdAsServer = 0;
int fcom = 0;

typedef void* thread_function(void*);

typedef struct  _ARG  {  
    int connfd;  
    struct sockaddr_in client;   
}ARG;                    //客户端结构体 

pthread_t createThread(thread_function thread)
{
	pthread_t tid;
    void *ret;
    int err = pthread_create(&tid, NULL, thread, NULL);
    if (err != 0)
    {
        perror("pthread_create\n");
    }
	return tid;
}

void* threadSerial2Socket(void*)
{
    // pthread_detach(pthread_self());//分离后仍可被等待
    printf("threadSerial2Socket pid is: %d, tid is: %d\n", getpid(),pthread_self());
	int rwnum = 0;
	char rwbuf[1024];
	while(1)
	{
		if((rwnum=ComReceive(fcom, rwbuf, 0)) > 0) 
		{
			rwbuf[rwnum]= 0;
			printf(rwbuf);
			send(sockfdClient, rwbuf, strlen(rwbuf), 0);
			printf(" Send %d byte(s) to TCP Server\n", strlen(rwbuf));
			fflush(stdout);
		}
	}

}

void process_cli(int connectfd, sockaddr_in client)  
{  
    int num;  
    char recvbuf[MAXDATASIZE], sendbuf[MAXDATASIZE], cli_name[MAXDATASIZE];  

    printf("You got a connection from %s.  ",inet_ntoa(client.sin_addr));        

    while (num = recv(connectfd, recvbuf, MAXDATASIZE,0)) {  
        recvbuf[num] = '\0';  
        ComSend(fcom,recvbuf);
    }  
	printf("Client disconnected");
    close(connectfd);   
}

void* start_routine(void* arg)  
{  
    ARG *info;  
    info = (ARG *)arg;  
      
    process_cli(info->connfd, info->client);  
    delete info;  
    pthread_exit(NULL);  
}  
void* threadSocket2Serial(void*)
{
    // pthread_detach(pthread_self());//分离后仍可被等待
    printf("threadSocket2Serial pid is: %d, tid is: %d\n", getpid(),pthread_self());
	int connect_fd = 0;
	char buff[4096];  
    int  n;  
	int sin_size= sizeof(struct sockaddr_in);  
	struct sockaddr_in client;     	//客户端地址信息结构体  
	ARG* arg;
	pthread_t  thread;        		//线程体变量  
	while(1)
	{
		//阻塞直到有客户端连接，不然多浪费CPU资源。  
		if((connect_fd = accept(sockfdAsServer, (struct sockaddr*)&client, (socklen_t *)&sin_size)) == -1){  
			printf("accept socket error: %s(errno: %d)",strerror(errno),errno);  
			continue;  
		}  

		arg = new  ARG;  
        arg->connfd = connect_fd;  
        memcpy(&arg->client, &client, sizeof(client));  
          
        if (pthread_create(&thread, NULL, start_routine, (void*)arg)) {        //创建线程，以客户端连接为参数，start_routine为线程执行函数  
            perror("Pthread_create() error");  
            continue;
        }
	}
}

int main(int argc, char **argv)
{
	int ret;
	struct sockaddr_in local_addr, server_addr;
	int comidx = 0;

	if(!Config::instance().init())
	{
		printf("config init failed \n");
		exit(1);
	}

	//init for client socket
	if(ret = (sockfdClient = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("creat socket failed \n");
		exit(1);
	}
	int portdata = Config::instance().getSocket().portRemoteServer;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portdata);
	server_addr.sin_addr.s_addr = inet_addr(Config::instance().getSocket().ip.c_str());
	
	printf("Destination IP: %s : %d\n", inet_ntoa(server_addr.sin_addr), portdata);	
	printf("Connecting...\n");

	if(ret = connect(sockfdClient, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("connect failed\n");
		exit(1);
	}
	else if(ret == 0)
	{
		printf("Conect success\n");
	}

	//init for server socket
	if(ret = (sockfdAsServer = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("creat socket failed \n");
		exit(1);
	}
	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(Config::instance().getSocket().portAsServer);
	local_addr.sin_addr.s_addr = INADDR_ANY;
	
	const int on=1;
	setsockopt(sockfdAsServer,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

	if(ret = bind(sockfdAsServer, (struct sockaddr *)&local_addr, sizeof(struct sockaddr)) < 0)
	{
		printf("bind failed \n");
		exit(1);
	}

	if(listen(sockfdAsServer,BACKLOG) == -1){      //调用listen，开始监听  
        perror("listen() error\n");  
        exit(1);  
    }

	std::string name = Config::instance().getCom().name;
	int baudrate = Config::instance().getCom().baudrate;
	int databit = Config::instance().getCom().databit;
	int stopbit = Config::instance().getCom().stopbit;
	int parity = Config::instance().getCom().parity;

	fcom = ComInit(name.c_str(), baudrate,databit,stopbit,parity);		
	if(fcom < 0)
	{
		exit(1);
	}
	

	pthread_t theadId1 = createThread(threadSerial2Socket);
	pthread_t theadId2 = createThread(threadSocket2Serial);

	//如果直接运行等待代码，一般会等待成功，返回1
    //如果在等待之前加入取消。等待错误，返回-1
    //  pthread_cancel(tid);
    //线程可以自我取消也可以被取消，线程终止
    //调用pthread_exit(tid);和取消同样用法。
    pthread_join(theadId1, NULL);
	pthread_join(theadId2, NULL);

	ComClose(fcom);
	close(sockfdClient);
	close(sockfdAsServer);

	exit(0);
}
