#pragma once

#include "config.h"
#include <vector>
#include <pthread.h>

class CServer{
    bool init(SSocketServer server,std::vector<pthread_t>& vecThreadId);

    bool initServer(int port);
    bool initCom(SCom com);
public:
    int mSocketid;
    int mComid;
    int mServerIndex;

    std::vector<int> mVecClientId;
};