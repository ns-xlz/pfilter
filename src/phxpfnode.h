//
// Created by xianglizhao on 2018/5/28.
//

#ifndef PFILTER2_PHXPFNODE_H
#define PFILTER2_PHXPFNODE_H

#include "log.h"
#include "phxpfsm.h"
#include "phxpaxos/node.h"

class PhxPartitionFilterNode {
private:
    phxpaxos::NodeInfo m_mynode;
    phxpaxos::NodeInfoList m_vecNodeList;
    phxpaxos::Node * m_paxosNode;
    PhxPartitionFilterSm m_phxpfSm;

    int m_batchSize;
    int m_batchTimeoutMs;
private:
    int PFPropose(const phxpf::PhxPFOperator & op, phxpf::PhxPFResponse &response);
    int PFPropose(const phxpf::PhxPFSingleOperator & op, phxpf::PhxPFSingleResponse &response);
    void PFBoradcast(phxpf::PhxMsgType msgType, const string & value);

    int GetGroupIdx() const;
public:
    PhxPartitionFilterNode();
    ~PhxPartitionFilterNode();
    int RunPaxos(const PhxPartitionFilterNodeConfig & config);
    bool Ready(const string * msg) const;
    bool IsMaster() const ;

    int Filter(const phxpf::PhxPFOperator & op, phxpf::PhxPFResponse &response);
    int Update(const phxpf::PhxPFOperator & op, phxpf::PhxPFResponse &response);
    int Delete(const phxpf::PhxPFOperator & op, phxpf::PhxPFResponse &response);
    int FilterOne(const phxpf::PhxPFSingleOperator & op, phxpf::PhxPFSingleResponse &response);
    int UpdateOne(const phxpf::PhxPFSingleOperator & op, phxpf::PhxPFSingleResponse &response);
    int DeleteOne(const phxpf::PhxPFSingleOperator & op, phxpf::PhxPFSingleResponse &response);
};


#endif //PFILTER2_PHXPFNODE_H
