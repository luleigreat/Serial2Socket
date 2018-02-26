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
	int portRemoteServer;
	int portAsServer;
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
	// int getSocketCount();
	SSocket getSocket();

	bool init();
private:
	int getParity(std::string sParity);
	int getStopbit(int stopbit);
private:
	SCom mCom;
	SSocket mSocket;
	// std::vector<SSocket> mVecSocket;
	// int mSocketCount;
};