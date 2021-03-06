//
// Created by xianglizhao on 2018/5/17.
//

#include "share_mapping.h"
#include <unistd.h>

SharedMapping::SharedMapping(string name, boost::interprocess::offset_t size, bool autoClose)
        : m_shareObjName(name), m_size(size), m_autoClose(autoClose) {
//    bool exist = exists(name);
    // do not use boost::filesystem::exists, it's a bug if you build boost by yourself
    bool exist = access(name.c_str(), 0) == 0;
    if (!exist) {
        boost::interprocess::file_mapping::remove(name.c_str());
        std::filebuf fbuf;
        fbuf.open(name.c_str(), std::ios_base::in | std::ios_base::out
                                | std::ios_base::trunc | std::ios_base::binary);
        fbuf.pubseekoff(size - 1, std::ios_base::beg);
        fbuf.sputc(0);
        fbuf.close();
    }
    m_mapping = new boost::interprocess::file_mapping(name.c_str(), boost::interprocess::read_write);
    m_region = new boost::interprocess::mapped_region(*m_mapping, boost::interprocess::read_write);
    m_data = m_region->get_address();
    if (!exist) {
        Clear();
    }
}

void SharedMapping::Clear() {
    std::memset(m_data, 0, m_size);
}

boost::interprocess::offset_t SharedMapping::Length() const {
    return m_size;
}

void *SharedMapping::Data() const {
    return m_data;
}

const string SharedMapping::Name() const {
    return m_shareObjName;
}

boost::interprocess::file_mapping *SharedMapping::Mapping() const {
    return m_mapping;
}

boost::interprocess::mapped_region *SharedMapping::Region() const {
    return m_region;
}

SharedMapping::~SharedMapping() {
    if (m_autoClose) {
        boost::interprocess::file_mapping::remove(m_shareObjName.c_str());
    }
    delete (m_mapping);
    delete (m_region);
}
