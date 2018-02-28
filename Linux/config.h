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

struct SSocketServer {
	int port;
	SCom com;
};

struct SSocketClient {
	std::string ip;
	int port;
	SCom com;
}


class Config
{
public:
	static Config& instance()
	{
		static Config config;
		return config;
	}

	~Config();

	std::vector<SSocketServer> getServerVector();
	std::vector<SSocketClient> getClientVector();

	bool init();

	SCom readCom(std::string section_name);
private:
	std::vector<SSocketServer> mVecServer;
	std::vector<SSocketClient> mVecClient;
};