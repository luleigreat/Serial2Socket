/************************************************************************

���ڽ��մ�����ϢȻ����Socketת����������

************************************************************************/

#pragma once

#include <windows.h>
#include "DataQueue.h"
#include "Config.h"

class CSocketSwitch
{
public:
	~CSocketSwitch();

	bool init(CDataQueue& queue);

	bool initSocket();
	bool initSocket(int index);
	bool openSocketThread();

	//UINT WINAPI listenThread(void* pParam);
	CDataQueue& queue()
	{
		return *m_pQueue;
	}

	SOCKET getSocket(int i)
	{
		return mSocket[i];
	}

	void setSocket(int index,SOCKET socket)
	{
		mSocket[index] = socket;
	}
private:
	CDataQueue* m_pQueue;
	std::vector<SOCKET> mSocket;

	/** �߳̾�� */
	volatile HANDLE    m_hListenThread = INVALID_HANDLE_VALUE;
};
