#include "comdev.h"
#include "config.h"	
#include <pthread.h>
#include <queue>
#include <string>
#include "CServer.h"
#include "CClient.h"
#include <signal.h>


//全局变量定义

std::vector<pthread_t> g_vecThreadId;
std::vector<CServer*> g_vecServer;
std::vector<CClient*> g_vecClient;

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
