#include <windows.h>
#include "CSerial2Socket.h"
#include "Log.h"
#include "Config.h"
#include <process.h>


enum { EOF_Char = 27 };

bool CSerial2Socket::init(CDataQueue& queue)
{
	m_pQueue = &queue;

	if (!initSerial())
	{
		Log("init serial failed");
		return false;
	}

	//if (!openThread())
	//{
	//	Log("open serial thread failed");
	//	return false;
	//}

	return true;
}

bool CSerial2Socket::initSerial()
{
	LONG  lLastError = ERROR_SUCCESS;
	TCHAR wc[256];
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)Config::instance().getCom().name.c_str(), -1, wc, 256);
	// Attempt to open the serial port (COM1)
	lLastError = mSerial.Open(wc, 0, 0, false);
	if (lLastError != ERROR_SUCCESS)
	{
		return Log("Unable to open COM-port,Err code:", mSerial.GetLastError());
		return false;
	}

	// Setup the serial port (9600,8N1, which is the default setting)
	//Setup(CSerial::EBaud9600, CSerial::EData8, CSerial::EParNone, CSerial::EStop1);
	lLastError = mSerial.Setup(CSerial::EBaudrate(Config::instance().getCom().baudrate),
		CSerial::EDataBits(Config::instance().getCom().databit),
		CSerial::EParity(Config::instance().getCom().parity),
		CSerial::EStopBits(Config::instance().getCom().stopbit));
	if (lLastError != ERROR_SUCCESS)
	{
		Log("Unable to set COM-port setting,Err code:", mSerial.GetLastError());
		return false;
	}

	// Register only for the receive event
	lLastError = mSerial.SetMask(CSerial::EEventBreak |
		CSerial::EEventCTS |
		CSerial::EEventDSR |
		CSerial::EEventError |
		CSerial::EEventRing |
		CSerial::EEventRLSD |
		CSerial::EEventRecv);
	if (lLastError != ERROR_SUCCESS)
	{
		Log("Unable to set COM-port event mask", mSerial.GetLastError());
		return false;
	}

	// Use 'non-blocking' reads, because we don't know how many bytes
	// will be received. This is normally the most convenient mode
	// (and also the default mode for reading data).
	lLastError = mSerial.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
	if (lLastError != ERROR_SUCCESS)
	{
		Log("Unable to set COM-port read timeout.", mSerial.GetLastError());
		return false;
	}

	Log("serial init success!");

	CSerial& serial = mSerial;
	lLastError = ERROR_SUCCESS;
	// Keep reading data, until an EOF (CTRL-Z) has been received
	bool fContinue = true;
	do
	{
		// Wait for an event
		lLastError = serial.WaitEvent();
		if (lLastError != ERROR_SUCCESS)
			return Log("Unable to wait for a COM-port event.", serial.GetLastError());

		// Save event
		const CSerial::EEvent eEvent = serial.GetEventType();

		// Handle break event
		if (eEvent & CSerial::EEventBreak)
		{
			printf("\n### BREAK received ###\n");
		}

		// Handle CTS event
		if (eEvent & CSerial::EEventCTS)
		{
			printf("\n### Clear to send %s ###\n", serial.GetCTS() ? "on" : "off");
		}

		// Handle DSR event
		if (eEvent & CSerial::EEventDSR)
		{
			printf("\n### Data set ready %s ###\n", serial.GetDSR() ? "on" : "off");
		}

		// Handle error event
		if (eEvent & CSerial::EEventError)
		{
			printf("\n### ERROR: ");
			switch (serial.GetError())
			{
			case CSerial::EErrorBreak:		printf("Break condition");			break;
			case CSerial::EErrorFrame:		printf("Framing error");			break;
			case CSerial::EErrorIOE:		printf("IO device error");			break;
			case CSerial::EErrorMode:		printf("Unsupported mode");			break;
			case CSerial::EErrorOverrun:	printf("Buffer overrun");			break;
			case CSerial::EErrorRxOver:		printf("Input buffer overflow");	break;
			case CSerial::EErrorParity:		printf("Input parity error");		break;
			case CSerial::EErrorTxFull:		printf("Output buffer full");		break;
			default:						printf("Unknown");					break;
			}
			printf(" ###\n");
		}

		// Handle ring event
		if (eEvent & CSerial::EEventRing)
		{
			printf("\n### RING ###\n");
		}

		// Handle RLSD/CD event
		if (eEvent & CSerial::EEventRLSD)
		{
			printf("\n### RLSD/CD %s ###\n", serial.GetRLSD() ? "on" : "off");
		}

		// Handle data receive event
		if (eEvent & CSerial::EEventRecv)
		{
			// Read data, until there is nothing left
			DWORD dwBytesRead = 0;
			char szBuffer[1024];
			do
			{
				// Read data from the COM-port
				lLastError = serial.Read(szBuffer, sizeof(szBuffer) - 1, &dwBytesRead);
				if (lLastError != ERROR_SUCCESS)
					return Log("Unable to read from COM-port.", serial.GetLastError());

				if (dwBytesRead > 0)
				{
					// Finalize the data, so it is a valid string
					szBuffer[dwBytesRead] = '\0';

					queue().pushDataQueue(szBuffer);
					//int ret = send(g_socket, szBuffer, strlen(szBuffer), 0);
					//printf("sending data %s", szBuffer);
					//if (ret == SOCKET_ERROR)
					//{
					//	::ShowError(WSAGetLastError(), _T("Send data failed"));
					//}
					//// Display the data
					//printf("%s", szBuffer);

					// Check if EOF (CTRL+'[') has been specified
					if (strchr(szBuffer, EOF_Char))
						fContinue = false;
				}
			} while (dwBytesRead == sizeof(szBuffer) - 1);
		}
	} while (fContinue);

	// Close the port again
	serial.Close();

	return true;
}

//socket线程
UINT WINAPI listenSerialSocketThread(void* pParam)
{
	CSerial2Socket& self = *(CSerial2Socket*)pParam;
	//CSerial& serial = self.serial();

	CSerial serial;
	LONG  lLastError = ERROR_SUCCESS;
	TCHAR wc[256];
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)Config::instance().getCom().name.c_str(), -1, wc, 256);
	// Attempt to open the serial port (COM1)
	lLastError = serial.Open(wc, 0, 0, false);
	if (lLastError != ERROR_SUCCESS)
	{
		return Log("Unable to open COM-port,Err code:", serial.GetLastError());
		return false;
	}

	// Setup the serial port (9600,8N1, which is the default setting)
	//Setup(CSerial::EBaud9600, CSerial::EData8, CSerial::EParNone, CSerial::EStop1);
	lLastError = serial.Setup(CSerial::EBaudrate(Config::instance().getCom().baudrate),
		CSerial::EDataBits(Config::instance().getCom().databit),
		CSerial::EParity(Config::instance().getCom().parity),
		CSerial::EStopBits(Config::instance().getCom().stopbit));
	if (lLastError != ERROR_SUCCESS)
	{
		Log("Unable to set COM-port setting,Err code:", serial.GetLastError());
		return false;
	}

	// Register only for the receive event
	lLastError = serial.SetMask(CSerial::EEventBreak |
		CSerial::EEventCTS |
		CSerial::EEventDSR |
		CSerial::EEventError |
		CSerial::EEventRing |
		CSerial::EEventRLSD |
		CSerial::EEventRecv);
	if (lLastError != ERROR_SUCCESS)
	{
		Log("Unable to set COM-port event mask", serial.GetLastError());
		return false;
	}

	// Use 'non-blocking' reads, because we don't know how many bytes
	// will be received. This is normally the most convenient mode
	// (and also the default mode for reading data).
	lLastError = serial.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
	if (lLastError != ERROR_SUCCESS)
	{
		Log("Unable to set COM-port read timeout.", serial.GetLastError());
		return false;
	}


	lLastError = ERROR_SUCCESS;
	// Keep reading data, until an EOF (CTRL-Z) has been received
	bool fContinue = true;
	do
	{
		// Wait for an event
		lLastError = serial.WaitEvent();
		if (lLastError != ERROR_SUCCESS)
			return Log("Unable to wait for a COM-port event.", serial.GetLastError());

		// Save event
		const CSerial::EEvent eEvent = serial.GetEventType();

		// Handle break event
		if (eEvent & CSerial::EEventBreak)
		{
			printf("\n### BREAK received ###\n");
		}

		// Handle CTS event
		if (eEvent & CSerial::EEventCTS)
		{
			printf("\n### Clear to send %s ###\n", serial.GetCTS() ? "on" : "off");
		}

		// Handle DSR event
		if (eEvent & CSerial::EEventDSR)
		{
			printf("\n### Data set ready %s ###\n", serial.GetDSR() ? "on" : "off");
		}

		// Handle error event
		if (eEvent & CSerial::EEventError)
		{
			printf("\n### ERROR: ");
			switch (serial.GetError())
			{
			case CSerial::EErrorBreak:		printf("Break condition");			break;
			case CSerial::EErrorFrame:		printf("Framing error");			break;
			case CSerial::EErrorIOE:		printf("IO device error");			break;
			case CSerial::EErrorMode:		printf("Unsupported mode");			break;
			case CSerial::EErrorOverrun:	printf("Buffer overrun");			break;
			case CSerial::EErrorRxOver:		printf("Input buffer overflow");	break;
			case CSerial::EErrorParity:		printf("Input parity error");		break;
			case CSerial::EErrorTxFull:		printf("Output buffer full");		break;
			default:						printf("Unknown");					break;
			}
			printf(" ###\n");
		}

		// Handle ring event
		if (eEvent & CSerial::EEventRing)
		{
			printf("\n### RING ###\n");
		}

		// Handle RLSD/CD event
		if (eEvent & CSerial::EEventRLSD)
		{
			printf("\n### RLSD/CD %s ###\n", serial.GetRLSD() ? "on" : "off");
		}

		// Handle data receive event
		if (eEvent & CSerial::EEventRecv)
		{
			// Read data, until there is nothing left
			DWORD dwBytesRead = 0;
			char szBuffer[1024];
			do
			{
				// Read data from the COM-port
				lLastError = serial.Read(szBuffer, sizeof(szBuffer) - 1, &dwBytesRead);
				if (lLastError != ERROR_SUCCESS)
					return Log("Unable to read from COM-port.", serial.GetLastError());

				if (dwBytesRead > 0)
				{
					// Finalize the data, so it is a valid string
					szBuffer[dwBytesRead] = '\0';

					self.queue().pushDataQueue(szBuffer);
					//int ret = send(g_socket, szBuffer, strlen(szBuffer), 0);
					//printf("sending data %s", szBuffer);
					//if (ret == SOCKET_ERROR)
					//{
					//	::ShowError(WSAGetLastError(), _T("Send data failed"));
					//}
					//// Display the data
					//printf("%s", szBuffer);

					// Check if EOF (CTRL+'[') has been specified
					if (strchr(szBuffer, EOF_Char))
						fContinue = false;
				}
			} while (dwBytesRead == sizeof(szBuffer) - 1);
		}
	} while (fContinue);

	// Close the port again
	serial.Close();

	return 0;
}

bool CSerial2Socket::openThread()
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
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, listenSerialSocketThread, (void*)this, 0, &threadId);
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





