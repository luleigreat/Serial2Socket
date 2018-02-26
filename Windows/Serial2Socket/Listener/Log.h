#ifndef SERIAL_LOG_H
#define SERIAL_LOG_H

#include <string>

std::string GetProgramDir();

int Log(std::string msg);
int Log(std::string msg, int code);

#endif