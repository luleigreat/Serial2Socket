#include "CSocketServer.h"
#include "Log.h"
#include "Config.h"
#include <windows.h>
#include <process.h>

bool CSocketServer::init()
{
	//未配置关机码，直接返回成功
	if (!Config::instance().getShutDownInfo().needShutDown())
		return true;

	if (!initSocket())
		return false;
	if (!openServerThread())
		return false;

	return true;
}

bool CSocketServer::initSocket()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}

	//创建套接字    
	mSocket= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocket == INVALID_SOCKET)
	{
		Log("socket error !");
		return 0;
	}

	//绑定IP和端口    
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(Config::instance().getShutDownInfo().port);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(mSocket, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		Log("bind error !");
		return 0;
	}

	//开始监听    
	if (listen(mSocket, 5) == SOCKET_ERROR)
	{
		Log("listen error !");
		return 0;
	}

	return true;
}
UINT WINAPI clientThread(void* pParam)
{
	SOCKET sClient = (SOCKET)pParam;
	char revData[1024];
	int ret = 0;
	do {
		//接收数据    
		ret = recv(sClient, revData, 1024, 0);
		if (ret > 0)
		{
			revData[ret] = 0x00;
			std::string msg(revData);
			if (msg == Config::instance().getShutDownInfo().shutDownMsg)
			{
				//system("shutdown -s -t 2");//调用关机命令。
				Log("调用关机命令");
			}
		}
	} while (ret > 0);
	return 0;
}

//socket线程
UINT WINAPI listenThreadServer(void* pParam)
{
	CSocketServer& self = *(CSocketServer*)pParam;
	SOCKET socket = self.getSocket();
	//循环接收数据    
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	while (true)
	{
		sClient = accept(socket, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			Log("Failed to accept client!", WSAGetLastError());
			continue;
		}

		/** 线程ID */
		UINT threadId;
		/** 开启串口数据监听线程 */
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, clientThread, (void*)sClient, 0, &threadId);
		if (!hThread)
		{
			Log("Failed to create thread!");
			continue;
		}
		::CloseHandle(hThread);
	}

	closesocket(socket);

	return 0;
}

bool CSocketServer::openServerThread()
{
	/** 检测线程是否已经开启了 */
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** 线程已经开启 */
		return false;
	}

	/** 线程ID */
	UINT threadId;
	/** 开启串口数据监听线程 */
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, listenThreadServer, (void*)this, 0, &threadId);
	if (!m_hListenThread)
	{
		return false;
	}
	/** 设置线程的优先级,高于普通线程 */
	if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}

	return true;
}