/************************************************************************

@brief ���ڶ�ʱ������Ϣ����һ�����ӣ�֧��socket��http���֣�������

************************************************************************/
#pragma once

#include <windows.h>
#include "HttpConnect.h"

class CHeartBeatClient
{
public:
	~CHeartBeatClient();

	bool init();

	bool initSocket();

	bool openThread();


	SOCKET getSocket()
	{
		return mSocket;
	}

	void setSocket(SOCKET socket)
	{
		mSocket = socket;
	}
private:
	SOCKET mSocket = INVALID_SOCKET;
	HttpConnect mHttp;

	/** �߳̾�� */
	volatile HANDLE    m_hListenThread = INVALID_HANDLE_VALUE;
};
