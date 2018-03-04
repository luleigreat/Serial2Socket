#include "CHeartBeatClient.h"
#include "Log.h"
#include "Config.h"
#include <windows.h>
#include <process.h>

CHeartBeatClient::~CHeartBeatClient()
{

}

bool CHeartBeatClient::init()
{
	if (!Config::instance().getHeartBeat().needHeartBeat())
		return true;

	if (Config::instance().getHeartBeat().hasSocket())
	{
		if (!initSocket())
		{
			Log("init heart beat socket failed");
			return false;
		}			
	}

	//if (Config::instance().getHeartBeat().hasHttp())
	//{
	//	if (!mHttp.initSocket(Config::instance().getHeartBeat().http_host,Config::instance().getHeartBeat().http_port))
	//	{
	//		Log("init heart beat http socket failed");
	//		return false;
	//	}
	//}

	if (!openThread())
	{
		Log("open heart beat thread failed");
		return false;
	}
		
	return true;
}

// 初始化socket
bool CHeartBeatClient::initSocket()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}

	mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocket == INVALID_SOCKET)
	{
		Log("invalid socket!");
		return 0;
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(Config::instance().getHeartBeat().port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(Config::instance().getHeartBeat().ip.c_str());
	if (connect(mSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{  //连接失败   
		Log("connect error !");
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
		return 0;
	}

	return 1;
}

//socket线程
UINT WINAPI heartBeatThread(void* pParam)
{
	CHeartBeatClient& self = *(CHeartBeatClient*)pParam;
	HttpConnect http;
	do {
		if (Config::instance().getHeartBeat().hasSocket())
		{
			SOCKET socket = self.getSocket();
			std::string msg = Config::instance().getHeartBeat().msgSend;
			int ret = send(socket, msg.c_str(), msg.length(), 0);
			if (ret == SOCKET_ERROR)
			{
				//发送失败，尝试重连
				std::string err = "Send heartbeat data failed,Err code:" + WSAGetLastError();
				Log(err);
				self.setSocket(INVALID_SOCKET);
				while (self.getSocket() == INVALID_SOCKET)
				{
					int initRes = self.initSocket();
					if (initRes == 0)
						Sleep(2000);
				}
			}
		}

		if (Config::instance().getHeartBeat().hasHttp())
		{
			//http.postData(
			//	Config::instance().getHeartBeat().http_path,
			//	Config::instance().getHeartBeat().msgSend);
			//http.getData(
			//	Config::instance().getHeartBeat().http_path,
			//	Config::instance().getHeartBeat().http_get_paramname + "=" +
			//	Config::instance().getHeartBeat().msgSend);
			std::string ip = Config::instance().getHeartBeat().http_host;
			int port = Config::instance().getHeartBeat().http_port;
			std::string path = Config::instance().getHeartBeat().http_path;
			std::string content = Config::instance().getHeartBeat().http_get_paramname + "=" +
				Config::instance().getHeartBeat().msgSend;
			sendGetRequest(ip, port, path, content);
		}

		Sleep(Config::instance().getHeartBeat().milliSeconds);

	} while (true);

	return 0;
}

bool CHeartBeatClient::openThread()
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
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, heartBeatThread, (void*)this, 0, &threadId);
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