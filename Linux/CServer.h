#pragma once

#include "config.h"
#include <vector>
#include <pthread.h>

class CServer{
public:
    bool init(SSocketServer server,std::vector<pthread_t>& vecThreadId);
private:
    bool initServer(int port);
    bool initCom(SCom com);
public:
    int mSocketid;
    int mComid;
    int mServerIndex;

    std::vector<int> mVecClientId;
};