//
// Created by xianglizhao on 2018/6/1.
//

#include "log.h"
#include <glog/logging.h>

using namespace phxpaxos;

LoggerGuard::LoggerGuard()
        : m_pLogFunc(nullptr) {
}

LoggerGuard::~LoggerGuard() {
}

LoggerGuard *LoggerGuard::Instance() {
    static LoggerGuard oLoggerGuard;
    return &oLoggerGuard;
}

int LoggerGuard::Init(const std::string &moudleName, const std::string &logPath, const int logLevel, const int maxSize) {
    fLI::FLAGS_max_log_size = maxSize;
    int ret = LoggerGoogle::GetLogger(moudleName, logPath, logLevel, m_pLogFunc);
    if (ret != 0) {
        printf("get logger_google fail, ret %d\n", ret);
        return ret;
    }

    return 0;
}

phxpaxos::LogFunc LoggerGuard::GetLogFunc() {
    return m_pLogFunc;
}

