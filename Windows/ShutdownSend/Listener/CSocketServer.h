/************************************************************************

用于接收一个码然后关机，串口或者socket两种协议都可以支持，可以配置；

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

	/** 线程句柄 */
	volatile HANDLE    m_hListenThread = INVALID_HANDLE_VALUE;
};