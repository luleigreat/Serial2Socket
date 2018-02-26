#pragma once

#include <string>

struct SSerialShutDownInfo{
	int onoff;
	//�ػ���Ϣ
	std::string shutDownMsg;

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

struct SSocketShutDownInfo {
	int onoff;
	//�ػ���Ϣ
	std::string shutDownMsg;
	//�����˿�
	int port;

	bool needShutDown() const
	{
		return onoff == 1;
	}
};

struct SHeartBeat {
	//���������͵�����
	std::string msgSend;
	//���������ͼ�������룩
	int milliSeconds;
	//ip
	std::string ip;
	//port
	int port;

	std::string http_host;
	int http_port;
	std::string http_path;

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

	bool init();
private:
	int getParity(std::string sParity);
	int getStopbit(int stopbit);
private:
	SHeartBeat mHeartBeat;
	SSerialShutDownInfo mSerialShutdownInfo;
	SSocketShutDownInfo mShutDownInfo;
};