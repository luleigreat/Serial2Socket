#include "HttpConnect.h"
#include <windows.h>
#include <sstream> 
#include "Log.h"

#ifdef WIN32
#pragma comment(lib,"ws2_32.lib")
#endif

HttpConnect::HttpConnect()
{

}

bool HttpConnect::initSocket(std::string ip,int port)
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}

	mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocket == INVALID_SOCKET)
	{
		Log("invalid socket!");
		return 0;
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	if (connect(mSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{  //连接失败   
		Log("connect error !");
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
		return 0;
	}

	return 1;
}

HttpConnect::~HttpConnect()
{

}
void HttpConnect::socketHttp(std::string request)
{
	int ret = send(mSocket, request.c_str(), request.size(), 0);
	//if (ret == SOCKET_ERROR)
	//{
	//	//发送失败，尝试重连
	//	Log("Send heartbeat data failed,Err code:" , WSAGetLastError());
	//	mSocket = INVALID_SOCKET;
	//	while (mSocket == INVALID_SOCKET)
	//	{
	//		int initRes = initSocket(mIp,mPort);
	//		if (initRes == 0)
	//			Sleep(2000);
	//	}
	//}
}

void HttpConnect::postData(std::string path, std::string post_content)
{
	//POST请求方式
	std::stringstream stream;
	stream << "POST " << path;
	stream << " HTTP/1.0\r\n";
	stream << "Host: " << mIp << "\r\n";
	stream << "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
	stream << "Content-Type:application/x-www-form-urlencoded\r\n";
	stream << "Content-Length:" << post_content.length() << "\r\n";
	stream << "Connection:close\r\n\r\n";
	stream << post_content.c_str();

	socketHttp(stream.str());
}

void HttpConnect::getData(std::string path, std::string get_content)
{
	//GET请求方式
	std::stringstream stream;
	stream << "GET " << path << "?" << get_content;
	stream << " HTTP/1.0\r\n";
	stream << "Host: " << mIp << "\r\n";
	stream << "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
	stream << "Connection:close\r\n\r\n";

	socketHttp(stream.str());
}