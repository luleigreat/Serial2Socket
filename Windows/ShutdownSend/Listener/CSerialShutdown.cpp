#include "CSerialShutdown.h"
#include "Log.h"
#include "Config.h"
#include <process.h>
#include "MyVolumeCtrl.h"


enum { EOF_Char = 27 };

bool CSerialShutdown::init()
{	
	//未配置关机码，直接返回成功
	if (!Config::instance().getSerialShutdownInfo().needShutDown())
		return true;

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

bool CSerialShutdown::initSerial()
{
	LONG  lLastError = ERROR_SUCCESS;
	TCHAR wc[256];
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)Config::instance().getSerialShutdownInfo().name.c_str(), -1, wc, 256);
	// Attempt to open the serial port (COM1)
	lLastError = mSerial.Open(wc, 0, 0, false);
	Log(Config::instance().getSerialShutdownInfo().name);
	if (lLastError != ERROR_SUCCESS)
	{
		Log("Unable to open COM-port,Err code:", mSerial.GetLastError());
		return false;
	}

	// Setup the serial port (9600,8N1, which is the default setting)
	//Setup(CSerial::EBaud9600, CSerial::EData8, CSerial::EParNone, CSerial::EStop1);
	lLastError = mSerial.Setup(CSerial::EBaudrate(Config::instance().getSerialShutdownInfo().baudrate),
		CSerial::EDataBits(Config::instance().getSerialShutdownInfo().databit),
		CSerial::EParity(Config::instance().getSerialShutdownInfo().parity),
		CSerial::EStopBits(Config::instance().getSerialShutdownInfo().stopbit));
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

	Log("init shutdown serial success!");

	lLastError = ERROR_SUCCESS;
	// Keep reading data, until an EOF (CTRL-Z) has been received
	bool fContinue = true;
	do
	{
		// Wait for an event
		lLastError = mSerial.WaitEvent();
		if (lLastError != ERROR_SUCCESS)
			return Log("Unable to wait for a COM-port event.", mSerial.GetLastError());

		// Save event
		const CSerial::EEvent eEvent = mSerial.GetEventType();

		// Handle break event
		if (eEvent & CSerial::EEventBreak)
		{
			printf("\n### BREAK received ###\n");
		}

		// Handle CTS event
		if (eEvent & CSerial::EEventCTS)
		{
			printf("\n### Clear to send %s ###\n", mSerial.GetCTS() ? "on" : "off");
		}

		// Handle DSR event
		if (eEvent & CSerial::EEventDSR)
		{
			printf("\n### Data set ready %s ###\n", mSerial.GetDSR() ? "on" : "off");
		}

		// Handle error event
		if (eEvent & CSerial::EEventError)
		{
			printf("\n### ERROR: ");
			switch (mSerial.GetError())
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
			printf("\n### RLSD/CD %s ###\n", mSerial.GetRLSD() ? "on" : "off");
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
				lLastError = mSerial.Read(szBuffer, sizeof(szBuffer) - 1, &dwBytesRead);
				if (lLastError != ERROR_SUCCESS)
					return Log("Unable to read from COM-port.", mSerial.GetLastError());

				if (dwBytesRead > 0)
				{
					// Finalize the data, so it is a valid string
					szBuffer[dwBytesRead] = '\0';

					if (std::string(szBuffer) == Config::instance().getSerialShutdownInfo().shutDownMsg)
					{
						system("shutdown -s -t 2");//调用关机命令。
						Log("调用关机命令1");
					}
					else
					{

						//volume turn
						CMyVolumeCtrl myctrl;
						if (std::string(szBuffer) == Config::instance().getVolumeInfo().muteMsg)
						{
							if (myctrl.GetMute())
								myctrl.SetMute(false);
							else
								myctrl.SetMute(true);
						}
						else
						{
							int volume = myctrl.GetVolume();
							int nVol = 0;
							if (std::string(szBuffer) == Config::instance().getVolumeInfo().volumeUpMsg)
							{
								nVol = volume + Config::instance().getVolumeInfo().percent;
								int maxVolume = myctrl.GetMaxVol();
								if (nVol > maxVolume)
									nVol = maxVolume;
							}
							else if (std::string(szBuffer) == Config::instance().getVolumeInfo().volumeDownMsg)
							{
								nVol = volume - Config::instance().getVolumeInfo().percent;
								int minVolume = myctrl.GetMinVol();
								if (nVol < minVolume)
									nVol = minVolume;
							}
							if (nVol > 0 && myctrl.GetMute())
							{
								myctrl.SetMute(false);
							}

							myctrl.SetVolume(nVol);
						}
					}

					// Check if EOF (CTRL+'[') has been specified
					if (strchr(szBuffer, EOF_Char))
						fContinue = false;
				}
			} while (dwBytesRead == sizeof(szBuffer) - 1);
		}
	} while (fContinue);

	// Close the port again
	mSerial.Close();
	return true;
}

//bool CSerialShutdown::openThread()
//{
//	/** 检测线程是否已经开启了 */
//	if (m_hListenThread != INVALID_HANDLE_VALUE)
//	{
//		/** 线程已经开启 */
//		return false;
//	}
//
//	/** 线程ID */
//	UINT threadId;
//	/** 开启串口数据监听线程 */
//	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, listenSerialShutdownThread, (void*)this, 0, &threadId);
//	if (!m_hListenThread)
//	{
//		return false;
//	}
//	/** 设置线程的优先级,高于普通线程 */
//	if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
//	{
//		return false;
//	}
//
//	return true;
//}