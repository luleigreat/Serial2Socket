#pragma once

#include <windows.h>
#include "Serial.h"

class CSerialShutdown
{
public:
	bool init();

	bool initSerial();
	//bool openThread();

	CSerial& serial()
	{
		return mSerial;
	}
private:
	CSerial mSerial;
	/** Ïß³Ì¾ä±ú */
	volatile HANDLE    m_hListenThread = INVALID_HANDLE_VALUE;
};
