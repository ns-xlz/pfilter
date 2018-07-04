//
// Created by xianglizhao on 2018/6/1.
//

#ifndef PFILTER2_LOG_H
#define PFILTER2_LOG_H

#include "phxpaxos_plugin/logger_google.h"
#include <string>
#include <typeinfo>

#define LOGGER (LoggerGuard::Instance())
#define LOG_ERROR(format, args...)\
       if (LOGGER->GetLogFunc() != nullptr)phxpaxos::LoggerGoogle::LogError(format, ## args);
#define LOG_WARNING(format, args...)\
       if (LOGGER->GetLogFunc() != nullptr)phxpaxos::LoggerGoogle::LogWarning(format, ## args);
#define LOG_INFO(format, args...)\
       if (LOGGER->GetLogFunc() != nullptr)phxpaxos::LoggerGoogle::LogInfo(format, ## args);
#define LOG_VERBOSE(format, args...)\
       if (LOGGER->GetLogFunc() != nullptr)phxpaxos::LoggerGoogle::LogVerbose(format, ## args);

#define NLDebug(format, args...) LOG_VERBOSE("DEBUG: %s " format, __func__, ## args);
#define NLErr(format, args...) LOG_ERROR("ERR: %s " format, __func__, ## args);

#define PLErr(format, args...) LOG_ERROR("ERR: %s::%s " format, typeid(this).name(), __func__, ## args);
#define PLImp(format, args...) LOG_INFO("Showy: %s::%s " format, typeid(this).name(), __func__, ## args);
#define PLHead(format, args...) LOG_WARNING("Imp: %s::%s " format, typeid(this).name(), __func__, ## args);
#define PLDebug(format, args...) LOG_VERBOSE("DEBUG: %s::%s " format, typeid(this).name(), __func__, ## args);

class LoggerGuard {
public:
    LoggerGuard();

    ~LoggerGuard();

    int Init(const std::string &moduleName, const std::string &logPath, const int logLevel, const int maxSize);

    static LoggerGuard *Instance();

    phxpaxos::LogFunc GetLogFunc();

private:
    phxpaxos::LogFunc m_pLogFunc;
};

#endif //PFILTER2_LOG_H
