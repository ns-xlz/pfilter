//
// Created by xianglizhao on 2018/5/24.
//

#ifndef PFILTER2_COMMON_H
#define PFILTER2_COMMON_H
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/mutex.hpp>

//typedef boost::unique_lock<boost::mutex> ReadLock;
//typedef boost::unique_lock<boost::mutex> WriteLock;
typedef boost::unique_lock<boost::shared_mutex> WriteLock;
typedef boost::shared_lock<boost::shared_mutex> ReadLock;
typedef boost::mutex Mutex;

#endif //PFILTER2_COMMON_H
