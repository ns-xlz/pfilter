//
// Created by xianglizhao on 2018/5/25.
//

#ifndef PFILTER2_PHXPFCONFIG_H
#define PFILTER2_PHXPFCONFIG_H

#include "pfconfig.h"

struct Node {
    string o_ondeIp;
    unsigned int o_nodePort;
};

class PhxPartitionFilterNodeConfig {
private:
    int m_paxosIoThreadCnt;
    int m_paxosBatchSize;
    int m_paxosBatchTimeoutMs;
    bool m_paxosUseMaster;

    int m_logLevel;
    int m_maxLogSize; // mb
    Node m_paxosNode;
    vector<Node> m_paxosNodelist;
    string m_paxosLogPath;
    string m_sysLogPath;
    PartitionFilterConfig m_pfConfig;
    string m_rpcAddr;
public:
    PhxPartitionFilterNodeConfig();
    bool Read(const string & configFile);

    int PaxosIoThreadCount() const;
    int PaxosBatchSize() const;
    int PaxosBatchTimeoutMs() const;
    bool PaxosUserMaster() const;

    const Node & PaxosNodeInfo() const;
    const vector<Node> & PaxosNodeListInfo() const;
    const string & PaxosLogPath() const;
    const PartitionFilterConfig & PFConfig() const;
    const string & SysLogPath() const;
    int MaxLogSize() const;
    int LogLevel() const;
    const string & RpcAddr() const;

    void Print() const;
};

#endif //PFILTER2_PHXPFCONFIG_H
