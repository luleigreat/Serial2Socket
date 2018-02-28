#include "config.h"
#include "CServer.h"
#include "comdev.h"
#include "util.h"

#define MAXLINE 4096
#define MAXDATASIZE 1024    //缓冲区大小  
#define BACKLOG 50           //listen队列等待的连接数  

//客户端结构体 
typedef struct  _ARG  {  
    int connfd;
    CServer* pServer;
    struct sockaddr_in client;   
}ARG; 

void* threadCom2Client(void* arg)
{
	CServer* pServer = (CServer*)arg;
	char rwbuf[1024];
	while(1)
	{
		if((rwnum=ComReceive(fcom, rwbuf, 0)) > 0) 
		{
			rwbuf[rwnum]= 0;
            auto iter = pServer->mVecClient.begin();
			while( iter != pServer->mVecClientId.end())
            {
                int ret = send(*iter, rwbuf, strlen(rwbuf), 0);
                if(ret == SOCKET_ERROR)
                {
                    iter = pServer->mVecClientId.erase(iter);
                }
                else
                {
                    iter++;
                }
            }
		}
	}
}

void* threadSocketServer(void* param)
{
	CServer* pServer = (CServer*)param;
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
        arg->pServer = pServer;
        memcpy(&arg->client, &client, sizeof(client));  
          
        //add to vector
        pServer->mVecClient.push_back(connect_fd);

        if (pthread_create(&thread, NULL, start_routine, (void*)arg)) {        //创建线程，以客户端连接为参数，start_routine为线程执行函数  
            perror("Pthread_create() error");  
            continue;
        }
	}
}
void process_cli(ARG* info)  
{  
    int connectfd = info->connfd;
    sockaddr_in client = info->client;
    CServer* pServer = info->pServer;

    int num;  
    char recvbuf[MAXDATASIZE], sendbuf[MAXDATASIZE], cli_name[MAXDATASIZE];  

    printf("You got a connection from %s.  ",inet_ntoa(client.sin_addr));        

	while(1)
	{
		if (num = recv(connectfd, recvbuf, MAXDATASIZE,0)) 
		{  
			recvbuf[num] = '\0';  
			ComSend(pServer->mComid,recvbuf);
		}
		else
		{
			printf("Client disconnected");
			close(connectfd);   
			break;
		}
	}
}

void* start_routine(void* arg)  
{  
    ARG *info;  
    info = (ARG *)arg;  
      
    process_cli(info);  
    delete info;  
    pthread_exit(NULL);  
}  

bool CServer::init(SSocketServer server,std::vector<pthread_t>& vecThreadId)
{
    if(!initServer(server.port))
        return false;
    if(!initCom(server.com))
        return false;

    pthread_t threadId1 = createThread(threadCom2Client,this);
    pthread_t threadId2 = createThread(threadSocketServer,this);

    vecThreadId.push_back(threadId1);
    vecThreadId.push_back(threadId2);

    return true;
}

bool CServer::initServer(int port)
{
    //init for server socket
    if(ret = (mSocketid = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create socket failed \n");
        return false;
    }
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;
    
    const int on=1;
    setsockopt(mSocketid,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    if(ret = bind(mSocketid, (struct sockaddr *)&local_addr, sizeof(struct sockaddr)) < 0)
    {
        printf("bind failed \n");
        return false;
    }

    if(listen(mSocketid,BACKLOG) == -1){      //调用listen，开始监听  
        perror("listen() error\n");  
        return false; 
    }
    return true;
}

bool CServer::initCom(SCom com)
{
    std::string name = com.name;
    int baudrate = com.baudrate;
    int databit = com.databit;
    int stopbit = com.stopbit;
    int parity = com.parity;

    mComid = ComInit(name.c_str(), baudrate,databit,stopbit,parity);		
    if(mComid < 0)
    {
        return false;
    }
    return true;
}