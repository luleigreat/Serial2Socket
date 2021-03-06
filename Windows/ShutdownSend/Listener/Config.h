#pragma once

#include <string>

struct SSerialShutDownInfo{
	int onoff;
	//关机信息
	std::string shutDownMsg;
	
	//com
	std::string name;
	int baudrate;
	int databit;
	int stopbit;	
	int parity;

	bool needShutDown() const
	{
		return onoff == 1;
	}
};

struct SVolumeInfo {
	//音量调节相关
	std::string volumeUpMsg;
	std::string volumeDownMsg;
	std::string muteMsg;
	int			percent;
};

struct SSocketShutDownInfo {
	int onoff;
	//关机信息
	std::string shutDownMsg;
	//监听端口
	int port;

	bool needShutDown() const
	{
		return onoff == 1;
	}
};

struct SHeartBeat {
	//心跳包发送的内容
	std::string msgSend;
	//心跳包发送间隔（毫秒）
	int milliSeconds;
	//ip
	std::string ip;
	//port
	int port;

	std::string http_host;
	int http_port;
	std::string http_path;
	std::string http_get_paramname;

	int socket_on;
	int http_on;

	bool hasSocket() const
	{
		return socket_on == 1;
	}

	bool hasHttp()const
	{
		return http_on == 1;
	}

	bool needHeartBeat() const
	{
		return socket_on || http_on;
	}
};

struct STime {
	int reconnect_gap;
};

class Config
{
public:
	static Config& instance()
	{
		static Config config;
		return config;
	}

	const SHeartBeat& getHeartBeat();
	const SSocketShutDownInfo& getShutDownInfo();
	const SSerialShutDownInfo& getSerialShutdownInfo();
	const STime& getTimeConfig();
	const SVolumeInfo& getVolumeInfo();

	bool init();
	int getParity(std::string sParity);
	int getStopbit(int stopbit);
private:
	SHeartBeat mHeartBeat;
	SSerialShutDownInfo mSerialShutdownInfo;
	SSocketShutDownInfo mShutDownInfo;
	STime mReconnectTime;
	SVolumeInfo mVolume;
};