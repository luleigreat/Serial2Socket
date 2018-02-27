
#include "CSocketSwitch.h"
#include "Log.h"
#include <process.h>  

CSocketSwitch::~CSocketSwitch()
{
	for (int i = 0; i < mSocket.size(); i++)
	{
		::closesocket(mSocket[i]);
	}	
}

bool CSocketSwitch::init(CDataQueue& queue)
{
	m_pQueue = &queue;

	if (!initSocket())
		return false;
	if (!openSocketThread())
		return false;

	return true;
}

//��ʼ��socket
bool CSocketSwitch::initSocket()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}
	int nSocketCount = Config::instance().getSocketCount();
	mSocket.resize(nSocketCount);
	for (int i = 0; i <nSocketCount; i++)
	{
		if (!initSocket(i))
		{
			for (int j = 0; j < i; j++)
			{
				mSocket[i] = INVALID_SOCKET;
				closesocket(mSocket[j]);
			}

			return 0;
		}
	}

	Log("socket init success");
	return 1;
}

bool CSocketSwitch::initSocket(int i)
{
	mSocket[i] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocket[i] == INVALID_SOCKET)
	{
		Log("invalid socket!");
		return 0;
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(Config::instance().getSocket(i).port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(Config::instance().getSocket(i).ip.c_str());
	if (connect(mSocket[i], (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{  //����ʧ��   
		Log("connect error !");
		closesocket(mSocket[i]);
		mSocket[i] = INVALID_SOCKET;
		return 0;
	}
	return true;
}


//socket�߳�
UINT WINAPI listenThread(void* pParam)
{
	CSocketSwitch& self = *(CSocketSwitch*)pParam;
	std::queue<std::string>& queue = self.queue().getQueue();
	do {
		//��com������һֱ�ȴ�
		if (queue.empty())
		{
			Sleep(100);
			continue;
		}
		//ȡ��ͷԪ�ط���
		std::string str = queue.back();
		int nSocketCount = Config::instance().getSocketCount();
		for (int i = 0; i < nSocketCount; i++)
		{
			int ret = send(self.getSocket(i), str.c_str(), str.length(), 0);
			//printf("sending data %s", str.c_str());
			if (ret == SOCKET_ERROR)
			{
				//����ʧ�ܣ���������
				std::string err = "Send data failed,Err code:" + WSAGetLastError();
				Log(err);
				self.setSocket(i,INVALID_SOCKET);
				while (self.getSocket(i) == INVALID_SOCKET)
				{
					int initRes = self.initSocket(i);
					if (initRes == 0)
						Sleep(1000);
				}
			}
		}
		//����
		self.queue().popDataQueue();

	} while (true);

	return 0;
}

bool CSocketSwitch::openSocketThread()
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
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, listenThread, (void*)this, 0, &threadId);
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
