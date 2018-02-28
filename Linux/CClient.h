#pragma once

#include "config.h"
#include <vector>
#include <string>
#include <pthread.h>

class CClient{
public:
    CClient();
    bool init(SSocketClient client,std::vector<pthread_t>& vecThreadId);
private:
    bool initClient(std::string ip,int port);
    bool initCom(SCom com);
public:
    int mSocketid;
    int mComid;
    bool mSocketInited;
    std::string mIp;
    int mPort;
};