#include "config.h"
#include "CClient.h"
#include "comdev.h"
#include "util.h"
#include <unistd.h>

void* threadCom2Client2(void* arg)
{
	CClient* pClient = (CClient*)arg;
	char rwbuf[1024];
    int rwnum = 0;
    int fcom = pClient->mComid;
	while(1)
	{
		if((rwnum=ComReceive(fcom, rwbuf, 0)) > 0) 
		{
			rwbuf[rwnum]= 0;
            
            if(pClient->mSocketInited)
            {
                int ret = send(pClient->mSocketid, rwbuf, strlen(rwbuf), 0);
                if(ret <= 0)
                {
                    pClient->mSocketInited = false;
                    printf("send failed");
                }
            }
            
		}
	}
}

void* threadInitClient(void* arg)
{
    CClient* pClient = (CClient*)arg;
    while(1)
    {
        if(!pClient->mSocketInited)
        {
            pClient->initClient(pClient->mIp,pClient->mPort);
        }
        
        usleep(5000);
    }
}

void* threadSocketClient(void* arg)
{
    CClient* pClient = (CClient*)arg;
    // pthread_detach(pthread_self());//分离后仍可被等待
    printf("threadSerial2Socket pid is: %d, tid is: %d\n", getpid(),pthread_self());
	int rwnum = 0;
	char rwbuf[1024];
	while(1)
	{
        if(pClient->mSocketInited)
        {
            int num = recv(pClient->mSocketid, rwbuf, 1024,0);
            if (num > 0) 
            {  
                rwbuf[num] = '\0';  
                ComSend(pClient->mComid,rwbuf);
            }
            else if(num <= 0)
            {
                pClient->mSocketInited = false;
            }
        }		
	}
}

CClient::CClient()
{
    mSocketInited = false;
}

bool CClient::init(SSocketClient client,std::vector<pthread_t>& vecThreadId)
{
    if(!initClient(client.ip,client.port))
    {
        printf("init client failed");
    }
        
    if(!initCom(client.com))
    {
        printf("init com failed");
        return false;
    }
        

    pthread_t threadId1 = createThread(threadCom2Client2,this);
    pthread_t threadId2 = createThread(threadSocketClient,this);
    pthread_t threadId3 = createThread(threadInitClient,this);

    vecThreadId.push_back(threadId1);
    vecThreadId.push_back(threadId2);
    vecThreadId.push_back(threadId3);

    return true;
}

bool CClient::initClient(std::string ip,int port)
{
    //init for client socket
    int ret = 0;
    struct sockaddr_in server_addr;
    if(ret = (mSocketid = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create socket failed \n");
        exit(1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    
    printf("Destination IP: %s : %d\n", inet_ntoa(server_addr.sin_addr), port);	
    printf("Connecting...\n");

    if(ret = connect(mSocketid, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("connect failed\n");
        exit(1);
    }
    else if(ret == 0)
    {
        printf("client conect success\n");
        mSocketInited = true;
    }
}

bool CClient::initCom(SCom com)
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