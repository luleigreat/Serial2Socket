#include "config.h"
#include "zini.h"

std::vector<SSocketServer> Config::getServerVector()
{
	return mVecServer;
}

std::vector<SSocketClient> Config::getClientVector()
{
	return mVecClient;
}

SCom Config::readCom(std::string section_name)
{
	SCom com;
	com.name = ZIni::readString(section_name, "name", "/dev/ttyS0", "conf.ini");
	com.baudrate = ZIni::readInt(section_name, "baudrate", 115200, "conf.ini");
	com.databit = ZIni::readInt(section_name, "databit", 8, "conf.ini");
	com.stopbit = ZIni::readInt(section_name, "stopbit", 1, "conf.ini");
	std::string sParity = ZIni::readString(section_name, "parity", "n", "conf.ini");
	com.parity = sParity[0];
	
	return com;
}

bool Config::init()
{
	try {		
		int nServer = ZIni::readInt("Main", "count_of_server", 0, "conf.ini");
		int nClient = ZIni::readInt("Main", "count_of_client", 0, "conf.ini");

		if(nServer > 0 && nClient > 0)
		{
			printf("count_of_server and count_of_client can only one positive");
			return false;
		}
		if(nServer <= 0 && nClient <= 0)
		{
			printf("count_of_server and count_of_client must have one positive");
			return false;
		}

		for (int i = 0; i < nServer; i++)
		{
			std::string section_name = "Server" + std::to_string(i+1);
			SSocketServer server;
			server.port = ZIni::readInt(section_name, "port", 5000, "conf.ini");

			server.com = readCom(section_name);

			mVecServer.push_back(server);
		}


		for (int i = 0; i < nClient; i++)
		{
			std::string section_name = "Client" + std::to_string(i+1);
			SSocketClient client;
			client.ip = ZIni::readString(section_name, "ip" , "127.0.0.1", "conf.ini");
			client.port = ZIni::readInt(section_name, "port", 5000, "conf.ini");

			client.com = readCom(section_name);

			mVecClient.push_back(client);
		}

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