#include "comdev.h"
#include "config.h"	
#include <pthread.h>
#include <queue>
#include <string>
#include "CServer.h"
#include <signal.h>


//全局变量定义

std::vector<pthread_t> g_vecThreadId;
std::vector<SServer*> g_vecServer;
std::vector<SClient*> g_vecClient;

// void* threadSerial2Socket(void*)
// {
//     // pthread_detach(pthread_self());//分离后仍可被等待
//     printf("threadSerial2Socket pid is: %d, tid is: %d\n", getpid(),pthread_self());
// 	int rwnum = 0;
// 	char rwbuf[1024];
// 	while(1)
// 	{
// 		if((rwnum=ComReceive(fcom, rwbuf, 0)) > 0) 
// 		{
// 			rwbuf[rwnum]= 0;
// 			printf(rwbuf);
// 			send(sockfdClient, rwbuf, strlen(rwbuf), 0);
// 			printf(" Send %d byte(s) to TCP Server\n", strlen(rwbuf));
// 			fflush(stdout);
// 		}
// 	}

// }

int main(int argc, char **argv)
{
	int ret;
	struct sockaddr_in local_addr, server_addr;

	if(!Config::instance().init())
	{
		printf("config init failed \n");
		exit(1);
	}

	signal(SIGPIPE,SIG_IGN);
	auto vecClient = Config::instance().getClientVector();
	for(int i=0; i<vecClient.size(); i++)
	{
		CClient* pClient = new CClient();
		pClient->init(vecClient[i],g_vecThreadId);
		g_vecClient.push_back(pClient);
	}


	//for server
	auto vecServer = Config::instance().getServerVector();
	for(int i=0; i<vecServer.size(); i++)
	{

		CServer* pServer = new CServer();
		if(!pServer->init(vecServer[i],g_vecThreadId))
		{
			printf("int server %d failed",i);
			exit(0);
		}
		else
		{
			g_vecServer.push_back(pServer);
		}
	}

	//如果直接运行等待代码，一般会等待成功，返回1
    //如果在等待之前加入取消。等待错误，返回-1
    //  pthread_cancel(tid);
    //线程可以自我取消也可以被取消，线程终止
    //调用pthread_exit(tid);和取消同样用法。
	for(int i=0; i<g_vecThreadId.size(); i++)
	{
		pthread_join(g_vecThreadId[i],NULL);
	}

	for(int i=0; i<g_vecClient.size(); i++)
	{
		delete g_vecClient[i];
	}
	for(int i=0; i<g_vecServer.size(); i++)
	{
		delete g_vecServer[i];
	}
	exit(0);
}
