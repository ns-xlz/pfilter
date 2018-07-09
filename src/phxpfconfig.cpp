//
// Created by xianglizhao on 2018/5/25.
//

#include "phxpfconfig.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

PhxPartitionFilterNodeConfig::PhxPartitionFilterNodeConfig() {

}

bool PhxPartitionFilterNodeConfig::Read(const string & configFile) {
    ptree pt;
    read_json(configFile, pt);

    m_paxosIoThreadCnt = pt.get<int>("io_thread_count", 4);
    m_paxosBatchSize = pt.get<int>("batch_size", 10);
    m_paxosBatchTimeoutMs = pt.get<int>("batch_timeout_ms", 10);
    m_paxosUseMaster = pt.get<int>("user_master", 0) != 0;

    m_logLevel = pt.get<int>("log_level", 0);
    m_maxLogSize = pt.get<int>("max_log_size", 128);
    m_rpcAddr = pt.get<string>("rpc_addr", "127.0.0.1:12345");

    string mynode = pt.get<string>("nodeaddr", "127.0.0.1:22345");
    vector<string> addr;
    boost::split(addr, mynode, boost::is_any_of(":,"), boost::token_compress_on);
    if (addr.size() != 2) {
	    std::cout<<mynode<<" split not size 2, size: "<<addr.size()<<std::endl;
        return false;
    }
    m_paxosNode.o_ondeIp = addr[0];
    m_paxosNode.o_nodePort = boost::lexical_cast<unsigned int>(addr[1]);

    string nodelist = pt.get<string>("nodelist");
    vector<string> addrs;
    boost::split(addrs, nodelist, boost::is_any_of(":,"), boost::token_compress_on);
    if (addrs.size() < 1 || addrs.size() % 2 != 0) {
	std::cout<<nodelist<<" split to size: "<<addrs.size()<<std::endl;
        return false;
    }

    for(size_t i = 0; i < addrs.size(); i += 2) {
        Node node;
        node.o_ondeIp = addrs[i];
        node.o_nodePort = boost::lexical_cast<unsigned int>(addrs[i+1]);
        m_paxosNodelist.push_back(node);
    }

    m_paxosLogPath = pt.get<string>("paxos_log_path");
    string pfConfigFile = pt.get<string>("pf_config_file");
    m_sysLogPath = pt.get<string>("sys_log_path");
    return m_pfConfig.Read(pfConfigFile);
}

int PhxPartitionFilterNodeConfig::PaxosIoThreadCount() const {
    return m_paxosIoThreadCnt;
}

const Node& PhxPartitionFilterNodeConfig::PaxosNodeInfo() const {
    return m_paxosNode;
}

const string& PhxPartitionFilterNodeConfig::PaxosLogPath() const {
    return m_paxosLogPath;
}

bool PhxPartitionFilterNodeConfig::PaxosUserMaster() const {
    return m_paxosUseMaster;
}

const vector<Node>& PhxPartitionFilterNodeConfig::PaxosNodeListInfo() const {
    return m_paxosNodelist;
}

const PartitionFilterConfig & PhxPartitionFilterNodeConfig::PFConfig() const {
    return m_pfConfig;
}

const string& PhxPartitionFilterNodeConfig::SysLogPath() const {
    return m_sysLogPath;
}

int PhxPartitionFilterNodeConfig::PaxosBatchSize() const {
    return m_paxosBatchSize;
}

int PhxPartitionFilterNodeConfig::PaxosBatchTimeoutMs() const {
    return m_paxosBatchTimeoutMs;
}

void PhxPartitionFilterNodeConfig::Print() const {
    std::cout<<"paxos io thread count: "<<m_paxosIoThreadCnt<<std::endl;
    std::cout<<"paxos batch count: "<<m_paxosBatchSize<<std::endl;
    std::cout<<"paxos batch timeout ms: "<<m_paxosBatchTimeoutMs<<std::endl;

    std::cout<<"phxpf log level: "<<m_logLevel<<std::endl;
    std::cout<<"phxpf log max size: "<<m_maxLogSize<<"mb"<<std::endl;
    std::cout<<"paxos node: "<<m_paxosNode.o_ondeIp<<":"<<m_paxosNode.o_nodePort<<std::endl;
    std::cout<<"paxos group list:"<<std::endl;
    for (auto i : m_paxosNodelist) {
        std::cout<<i.o_ondeIp<<":"<<i.o_nodePort<<std::endl;
    }
    std::cout<<"paxos log path: "<<m_paxosLogPath<<std::endl;
    std::cout<<"paxos sys log path: "<<m_sysLogPath<<std::endl;

    m_pfConfig.Print();
}

int PhxPartitionFilterNodeConfig::MaxLogSize() const {
    return m_maxLogSize;
}

int PhxPartitionFilterNodeConfig::LogLevel() const {
    return m_logLevel;
}

const string & PhxPartitionFilterNodeConfig::RpcAddr() const {
    return m_rpcAddr;
}