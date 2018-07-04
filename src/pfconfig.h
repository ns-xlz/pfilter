//
// Created by xianglizhao on 2018/6/27.
//

#ifndef PHXPF_PFCONFIG_H
#define PHXPF_PFCONFIG_H

#include <iostream>
#include <vector>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost::property_tree;

class PartitionFilterConfig {
private:
    string m_instancePath;
    string m_bloomPath;
    string m_bloomHashFunc;
    size_t m_bloomMaxPartitions;
    uint64_t m_bloomMaxItemSize;
    double m_bloomErrorRate;
    unsigned int m_bloomHashSeed;
//    string m_bloomHash
public:
    PartitionFilterConfig();
    bool Read(const string & configFile);

    const string & InstancePath() const;
    const string & BloomPath() const;
    const string & BloomHashFunc() const;
    size_t BloomMaxPartitions() const;
    uint64_t BloomMaxItems() const;
    double BloomErrorRate() const;
    unsigned int BloomHashSeed() const;

    void Print() const;
};


#endif //PHXPF_PFCONFIG_H
