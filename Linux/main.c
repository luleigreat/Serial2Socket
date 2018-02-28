#include "comdev.h"
#include "config.h"	
#include <pthread.h>
#include <queue>
#include <string>
#include "CServer.h"



//全局变量定义

std::vector<pthread_t> g_vecThreadId;

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


	// auto vecClient = Config::instance().getClientVector();
	// for(int i=0; i<vecClient.size(); i++)
	// {
	// 	//init for client socket
	// 	if(ret = (sockfdClient = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	// 	{
	// 		printf("creat socket failed \n");
	// 		exit(1);
	// 	}
	// 	int portdata = vecClient[i].port;
	// 	memset(&server_addr, 0, sizeof(server_addr));
	// 	server_addr.sin_family = AF_INET;
	// 	server_addr.sin_port = htons(portdata);
	// 	server_addr.sin_addr.s_addr = inet_addr(vecClient[i].ip.c_str());
		
	// 	printf("Destination IP: %s : %d\n", inet_ntoa(server_addr.sin_addr), portdata);	
	// 	printf("Connecting...\n");

	// 	if(ret = connect(sockfdClient, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	// 	{
	// 		printf("connect failed\n");
	// 		exit(1);
	// 	}
	// 	else if(ret == 0)
	// 	{
	// 		printf("Conect success\n");
	// 	}
	// }


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

	exit(0);
}
