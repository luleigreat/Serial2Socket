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
		/** 进入临界段 */
		EnterCriticalSection(&mCommunicationSync);

		mQueue.push(data);

		/** 退出临界区 */
		LeaveCriticalSection(&mCommunicationSync);
	}

	void popDataQueue()
	{
		/** 进入临界段 */
		EnterCriticalSection(&mCommunicationSync);

		mQueue.pop();

		/** 退出临界区 */
		LeaveCriticalSection(&mCommunicationSync);
	}
private:
	std::queue<std::string> mQueue;
	/** 同步互斥,临界区保护 */
	CRITICAL_SECTION   mCommunicationSync;       //!< 互斥操作串口    
};
