#pragma once
#include "Serial.h"
#include "DataQueue.h"

class CSerial2Socket
{
public:
	bool init(CDataQueue& queue);

	bool initSerial();
	bool openThread();

	CDataQueue& queue()
	{
		return *m_pQueue;
	}

	CSerial& serial()
	{
		return mSerial;
	}
private:
	CDataQueue* m_pQueue;
	CSerial mSerial;
	/** Ïß³Ì¾ä±ú */
	volatile HANDLE    m_hListenThread = INVALID_HANDLE_VALUE;
};