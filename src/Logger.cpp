#include <cstdarg>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

#include "Logger.h"

static std::mutex mtx;
void Logger::log(const char* msg, ...) {
    mtx.lock();

    va_list args;
    va_start(args, msg);

    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char timestr[64];
    strftime(timestr, 64, "%H:%M:%S", timeinfo);

    char buf1[1024], buf2[4096];
    sprintf(buf1, "[%s] %s", timestr, msg);
    vsprintf(buf2, buf1, args);

    std::cout << buf2 << std::endl;
    std::ofstream o;
    o.open("Vulpes.log", std::ofstream::out | std::ofstream::app);
    o << buf2 << std::endl;
    o.close();

    mtx.unlock();
}
