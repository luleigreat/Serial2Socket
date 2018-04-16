#include "CSocketServer.h"
#include "Log.h"
#include "Config.h"
#include <windows.h>
#include <process.h>

bool CSocketServer::init()
{
	//δ���ùػ��룬ֱ�ӷ��سɹ�
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

	//�����׽���    
	mSocket= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocket == INVALID_SOCKET)
	{
		Log("socket error !");
		return 0;
	}

	//��IP�Ͷ˿�    
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(Config::instance().getShutDownInfo().port);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(mSocket, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		Log("bind error !");
		return 0;
	}

	//��ʼ����    
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
		//��������    
		ret = recv(sClient, revData, 1024, 0);
		if (ret > 0)
		{
			revData[ret] = 0x00;
			std::string msg(revData);
			if (msg == Config::instance().getShutDownInfo().shutDownMsg)
			{
				//system("shutdown -s -t 2");//���ùػ����
				Log("���ùػ�����");
			}
		}
	} while (ret > 0);
	return 0;
}

//socket�߳�
UINT WINAPI listenThreadServer(void* pParam)
{
	CSocketServer& self = *(CSocketServer*)pParam;
	SOCKET socket = self.getSocket();
	//ѭ����������    
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

		/** �߳�ID */
		UINT threadId;
		/** �����������ݼ����߳� */
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
	/** ����߳��Ƿ��Ѿ������� */
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** �߳��Ѿ����� */
		return false;
	}

	/** �߳�ID */
	UINT threadId;
	/** �����������ݼ����߳� */
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, listenThreadServer, (void*)this, 0, &threadId);
	if (!m_hListenThread)
	{
		return false;
	}
	/** �����̵߳����ȼ�,������ͨ�߳� */
	if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}

	return true;
}