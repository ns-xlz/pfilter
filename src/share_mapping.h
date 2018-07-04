//
// Created by xianglizhao on 2018/5/17.
//

#ifndef PFILTER2_SHARE_MAPPING_H
#define PFILTER2_SHARE_MAPPING_H

#include <iostream>
#include <string>
#include <boost/core/noncopyable.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/filesystem.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

using namespace std;
using namespace boost::interprocess;
using namespace boost::filesystem;

class SharedMapping : public boost::noncopyable {
private:
    const string m_shareObjName;
    file_mapping * m_mapping;
    mapped_region * m_region;

    void * m_data;
    offset_t m_size;
    bool m_autoClose;
public:
    SharedMapping(string name, offset_t size, bool autoClose=false);

    void Clear();
    offset_t Length() const;
    void * Data() const;
    const string Name() const;
    file_mapping* Mapping() const;
    mapped_region* Region() const;

    ~SharedMapping();
};



#endif //PFILTER2_SHARE_MAPPING_H
