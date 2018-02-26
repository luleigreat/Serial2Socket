#include "config.h"
#include "zini.h"

const SCom& Config::getCom()
{
	return mCom;
}
SSocket Config::getSocket()
{
	//return mVecSocket[index];
	return mSocket;
}

// int Config::getSocketCount()
// {
// 	return mSocketCount;
// }
bool Config::init()
{
	try {
		mCom.name = ZIni::readString("Serial", "name", "/dev/ttyS0", "conf.ini");
		mCom.baudrate = ZIni::readInt("Serial", "baudrate", 115200, "conf.ini");
		mCom.databit = ZIni::readInt("Serial", "databit", 8, "conf.ini");
		mCom.stopbit = ZIni::readInt("Serial", "stopbit", 1, "conf.ini");
		std::string sParity = ZIni::readString("Serial", "parity", "n", "conf.ini");
		mCom.parity = sParity[0];

		mSocket.ip = ZIni::readString("Socket", "ipOfServer", "127.0.0.1", "conf.ini");
		mSocket.portRemoteServer = ZIni::readInt("Socket", "portRemoteServer", 5000, "conf.ini");
		mSocket.portAsServer = ZIni::readInt("Socket","portAsServer",5000,"conf.ini");

		
		// mSocketCount = ZIni::readInt("Socket", "count", 0, "conf.ini");
		// if (mSocketCount == 0)
		// {
		// 	Log("'count' in 'Socket' section cannot be 0");
		// 	return false;
		// }

		// for (int i = 0; i < mSocketCount; i++)
		// {
		// 	SSocket socket;
		// 	socket.ip = ZIni::readString("Socket", "ip" + std::to_string(i+1), "127.0.0.1", "conf.ini");
		// 	socket.port = ZIni::readInt("Socket", "port" + std::to_string(i+1), 5000, "conf.ini");
		// 	mVecSocket.push_back(socket);
		// }

		return true;
	}
	catch (std::exception e)
	{
		// printf(e.what());
		return false;
	}
}

Config::~Config()
{
}