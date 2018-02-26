#pragma once

#include <windows.h>
#include <queue>

class CDataQueue
{
public:
	CDataQueue()
	{
		InitializeCriticalSection(&mCommunicationSync);
	}

	std::queue<std::string>& getQueue() 
	{
		return mQueue;
	}

	void pushDataQueue(std::string data)
	{
		/** �����ٽ�� */
		EnterCriticalSection(&mCommunicationSync);

		mQueue.push(data);

		/** �˳��ٽ��� */
		LeaveCriticalSection(&mCommunicationSync);
	}

	void popDataQueue()
	{
		/** �����ٽ�� */
		EnterCriticalSection(&mCommunicationSync);

		mQueue.pop();

		/** �˳��ٽ��� */
		LeaveCriticalSection(&mCommunicationSync);
	}
private:
	std::queue<std::string> mQueue;
	/** ͬ������,�ٽ������� */
	CRITICAL_SECTION   mCommunicationSync;       //!< �����������    
};
