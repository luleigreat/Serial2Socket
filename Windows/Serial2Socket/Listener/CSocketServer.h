/************************************************************************

���ڽ���һ����Ȼ��ػ������ڻ���socket����Э�鶼����֧�֣��������ã�

************************************************************************/

#pragma once

#include <windows.h>
#include "Log.h"

class CSocketServer
{
public:
	bool init();

	bool initSocket();

	bool openServerThread();

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

	/** �߳̾�� */
	volatile HANDLE    m_hListenThread = INVALID_HANDLE_VALUE;
};