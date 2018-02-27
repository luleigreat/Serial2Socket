#pragma once

#include <string>
#include <vector>

struct SCom {
	std::string name;
	int baudrate;
	int databit;
	int stopbit;
	int parity;
};

struct SSocket {
	std::string ip;
	int port;
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

	~Config();

	const SCom& getCom();
	int getSocketCount();
	SSocket getSocket(int index);
	const STime& getTimeConfig();

	bool init();
private:
	int getParity(std::string sParity);
	int getStopbit(int stopbit);
private:
	SCom mCom;
	std::vector<SSocket> mVecSocket;
	int mSocketCount;
	STime mReconnectTime;
};