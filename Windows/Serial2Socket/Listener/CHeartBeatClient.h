/************************************************************************

@brief 用于定时发送消息到另一个机子，支持socket与http两种，可配置

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

	/** 线程句柄 */
	volatile HANDLE    m_hListenThread = INVALID_HANDLE_VALUE;
};
