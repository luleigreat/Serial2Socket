#pragma once

#include <string>
#include <windows.h>

class HttpConnect
{
public:
	HttpConnect();
	~HttpConnect();

	bool initSocket(std::string ip, int port);

	void socketHttp(std::string request);

	void postData( std::string path, std::string post_content);

	void getData(std::string path, std::string get_content);

private:
	SOCKET mSocket;
	std::string mIp;
	int mPort;
};
void sendGetRequest(std::string ip, int port, std::string path, std::string get_content);