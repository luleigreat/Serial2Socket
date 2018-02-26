#include "Config.h"
#include "zini.h"
#include <windows.h>

const SHeartBeat& Config::getHeartBeat()
{
	return mHeartBeat;
}

const SSocketShutDownInfo& Config::getShutDownInfo()
{
	return mShutDownInfo;
}

const SSerialShutDownInfo& Config::getSerialShutdownInfo()
{
	return mSerialShutdownInfo;
}

bool Config::init()
{
	try {
		mShutDownInfo.onoff = ZIni::readInt("SocketShutDown", "on_off", 1, "conf.ini");
		mShutDownInfo.port = ZIni::readInt("SocketShutDown", "port", 0, "conf.ini");
		mShutDownInfo.shutDownMsg = ZIni::readString("SocketShutDown", "shutdown_msg", "", "conf.ini");

		
		mSerialShutdownInfo.shutDownMsg = ZIni::readString("SerialShutDown", "shutdown_msg", "", "conf.ini");
		mSerialShutdownInfo.onoff = ZIni::readInt("SerialShutDown", "on_off", 1, "conf.ini");
		mSerialShutdownInfo.name = ZIni::readString("SerialShutDown", "name", "COM1", "conf.ini");
		mSerialShutdownInfo.baudrate = ZIni::readInt("SerialShutDown", "baudrate", 9600, "conf.ini");
		mSerialShutdownInfo.databit = ZIni::readInt("SerialShutDown", "databit", 8, "conf.ini");
		int stopbit = ZIni::readInt("SerialShutDown", "stopbit", 1, "conf.ini");
		std::string sParity = ZIni::readString("SerialShutDown", "parity", "none", "conf.ini");
		mSerialShutdownInfo.parity = getParity(sParity);
		mSerialShutdownInfo.stopbit = getStopbit(stopbit);

		mHeartBeat.milliSeconds = ZIni::readInt("HeartBeat", "milliseconds", 0, "conf.ini");
		mHeartBeat.msgSend = ZIni::readString("HeartBeat", "heartbeat_msg", "", "conf.ini");
		mHeartBeat.socket_on = ZIni::readInt("HeartBeat", "socket_on", 1, "conf.ini");
		mHeartBeat.http_on = ZIni::readInt("HeartBeat", "http_on", 1, "conf.ini");
		mHeartBeat.ip = ZIni::readString("HeartBeat", "socket_ip", "", "conf.ini"); 
		mHeartBeat.port = ZIni::readInt("HeartBeat", "socket_port", 0, "conf.ini");
		mHeartBeat.http_host = ZIni::readString("HeartBeat", "http_ip", "", "conf.ini");
		mHeartBeat.http_port = ZIni::readInt("HeartBeat", "http_port", 80, "conf.ini");
		mHeartBeat.http_path = ZIni::readString("HeartBeat", "http_path", "", "conf.ini");
		return true;
	}
	catch (std::exception e)
	{
		printf(e.what());
		return false;
	}
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