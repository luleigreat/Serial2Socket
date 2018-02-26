#include "Config.h"
#include "zini.h"
#include <windows.h>
#include "Log.h"

const SCom& Config::getCom()
{
	return mCom;
}
SSocket Config::getSocket(int index)
{
	return mVecSocket[index];
}

int Config::getSocketCount()
{
	return mSocketCount;
}
bool Config::init()
{
	try {
		mCom.name = ZIni::readString("Serial", "name", "COM1", "conf.ini");
		mCom.baudrate = ZIni::readInt("Serial", "baudrate", 9600, "conf.ini");
		mCom.databit = ZIni::readInt("Serial", "databit", 8, "conf.ini");
		int stopbit = ZIni::readInt("Serial", "stopbit", 1, "conf.ini");
		std::string sParity = ZIni::readString("Serial", "parity", "none", "conf.ini");
		mCom.parity = getParity(sParity);
		mCom.stopbit = getStopbit(stopbit);

		mSocketCount = ZIni::readInt("Socket", "count", 0, "conf.ini");
		if (mSocketCount == 0)
		{
			Log("'count' in 'Socket' section cannot be 0");
			return false;
		}

		for (int i = 0; i < mSocketCount; i++)
		{
			SSocket socket;
			socket.ip = ZIni::readString("Socket", "ip" + std::to_string(i+1), "127.0.0.1", "conf.ini");
			socket.port = ZIni::readInt("Socket", "port" + std::to_string(i+1), 5000, "conf.ini");
			mVecSocket.push_back(socket);
		}

		return true;
	}
	catch (std::exception e)
	{
		printf(e.what());
		return false;
	}
}

Config::~Config()
{
}

int Config::getParity(std::string parity)
{
	if (parity == "even")
		return EVENPARITY;
	if (parity == "odd")
		return ODDPARITY;
	if (parity == "mark")
		return MARKPARITY;
	if (parity == "space")
		return SPACEPARITY;
	return NOPARITY;
}

int Config::getStopbit(int bit)
{
	if (bit == 1.5)
		return ONE5STOPBITS;
	if (bit == 2)
		return TWOSTOPBITS;
	return ONESTOPBIT;
}