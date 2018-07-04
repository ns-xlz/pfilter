//
// Created by xianglizhao on 2018/5/28.
//

#include "log.h"
#include "phxpfnode.h"
#include "phx_msg.h"

PhxPartitionFilterNode::PhxPartitionFilterNode()
    : m_paxosNode(nullptr) {

}

PhxPartitionFilterNode::~PhxPartitionFilterNode() {
    if (m_paxosNode != nullptr) {
        delete m_paxosNode;
    }
}

int PhxPartitionFilterNode::RunPaxos(const PhxPartitionFilterNodeConfig &config) {
    m_batchSize = config.PaxosBatchSize();
    m_batchTimeoutMs = config.PaxosBatchTimeoutMs();

    m_mynode.SetIPPort(config.PaxosNodeInfo().o_ondeIp, config.PaxosNodeInfo().o_nodePort);
    bool succ = m_phxpfSm.Init(config.PFConfig());
    if (!succ) {
        return phxpf::PHXPARTITIONFILTER_SYS_FAIL;
    }

    const vector<Node> & nodes = config.PaxosNodeListInfo();
    for (size_t i = 0; i < nodes.size(); ++i) {
        phxpaxos::NodeInfo info;
        const Node & node = nodes[i];
        info.SetIPPort(node.o_ondeIp, node.o_nodePort);
        m_vecNodeList.push_back(info);
    }

    string rpcAddr = config.RpcAddr();
    phxpaxos::Options options;
    options.sLogStoragePath = config.PaxosLogPath();
    options.iGroupCount = 1;
    options.vecNodeInfoList = m_vecNodeList;
    options.oMyNode = m_mynode;
    options.bSync = true;
    options.bUseBatchPropose = true;
    options.iIOThreadCount = config.PaxosIoThreadCount();
    options.pMasterChangeCallback = [this, rpcAddr](
        const int groupid, const phxpaxos::NodeInfo & node, const uint64_t version){
        PLImp("group: %d changeed master to %s:%d, version: %d",
              groupid, node.GetIP().c_str(), node.GetPort(), version);
    };

    phxpaxos::GroupSMInfo smInfo;
    smInfo.iGroupIdx = GetGroupIdx();
    smInfo.vecSMList.push_back(&m_phxpfSm);
    smInfo.bIsUseMaster = true;
    options.vecGroupSMInfoList.push_back(smInfo);

    int ret = LOGGER->Init("PHXPF", config.SysLogPath(), config.LogLevel(), config.MaxLogSize());
    if (ret != 0) {
        PLErr("phxpf log init failed with ret: %d", ret);
        return ret;
    }
    options.pLogFunc = LOGGER->GetLogFunc();

    ret = phxpaxos::Node::RunNode(options, m_paxosNode);
    if (ret != 0) {
        PLErr("run phxpaxos node failed with ret: %d", ret);
    }
    m_paxosNode->SetBatchCount(GetGroupIdx(), m_batchSize);
    m_paxosNode->SetBatchDelayTimeMs(GetGroupIdx(), m_batchTimeoutMs);
    return ret;
}

void PhxPartitionFilterNode::PFBoradcast(phxpf::PhxMsgType msgType, const string &value) {
    phxpf::PhxPFMsg msg;
    msg.set_vtype(msgType);
    msg.set_value(value);

    uint64_t llinstanceId = 0;
    int ret = m_paxosNode->Propose(0, msg.SerializeAsString(), llinstanceId);
    if (ret != 0) {
        PLErr("paxos node boradcast failed with ret: %d", ret);
    }
}

int PhxPartitionFilterNode::GetGroupIdx() const {
    return 0;
}

int PhxPartitionFilterNode::PFPropose(const phxpf::PhxPFOperator &op, phxpf::PhxPFResponse &response) {
    phxpaxos::SMCtx ctx;
    ctx.m_iSMID = 1;
    ctx.m_pCtx = (void *)&response;

    uint64_t llinstanceId = 0;
    phxpf::PhxPFMsg msg;
    msg.set_vtype(phxpf::PhxMsgType::MULTI);
    msg.set_value(op.SerializeAsString());

    int groupIdx = GetGroupIdx();
    uint32_t batchId;
    int ret = m_paxosNode->BatchPropose(groupIdx, msg.SerializeAsString(), llinstanceId, batchId, &ctx);

    if (ret != 0) {
        PLErr("paxos node propose failed with ret: %d", ret);
    }
    return ret;
}

int PhxPartitionFilterNode::PFPropose(const phxpf::PhxPFSingleOperator &op, phxpf::PhxPFSingleResponse &response) {
    phxpaxos::SMCtx ctx;
    ctx.m_iSMID = 1;
    ctx.m_pCtx = (void *)&response;

    uint64_t llinstanceId = 0;
    phxpf::PhxPFMsg msg;
    msg.set_vtype(phxpf::PhxMsgType::SINGLE);
    msg.set_value(op.SerializeAsString());

    int groupIdx = GetGroupIdx();
    uint32_t batchId;
    int ret = m_paxosNode->BatchPropose(groupIdx, msg.SerializeAsString(), llinstanceId, batchId, &ctx);

    if (ret != 0) {
        PLErr("paxos node propose failed with ret: %d", ret);
    }
    return ret;
}

int PhxPartitionFilterNode::Filter(const phxpf::PhxPFOperator &op, phxpf::PhxPFResponse &response) {
    response.set_ret(phxpf::PHXPARTITIONFILTER_OK);
    m_phxpfSm.FilterInstance()->Filter(op.args(), *response.mutable_value());
    return phxpf::PHXPARTITIONFILTER_OK;
}

int PhxPartitionFilterNode::Update(const phxpf::PhxPFOperator &op, phxpf::PhxPFResponse &response) {
    if (m_paxosNode->IsIMMaster(GetGroupIdx())) {
        return PFPropose(op, response);
    }
    response.set_ret(phxpf::PHXPARTITIONFILTER_REDIRECT);
    return phxpf::PHXPARTITIONFILTER_NOTMASTER;
}

int PhxPartitionFilterNode::Delete(const phxpf::PhxPFOperator &op, phxpf::PhxPFResponse &response) {
    if (m_paxosNode->IsIMMaster(GetGroupIdx())) {
        return PFPropose(op, response);
    }
    response.set_ret(phxpf::PHXPARTITIONFILTER_REDIRECT);
    return phxpf::PHXPARTITIONFILTER_NOTMASTER;
}

int PhxPartitionFilterNode::FilterOne(const phxpf::PhxPFSingleOperator &op, phxpf::PhxPFSingleResponse &response) {
    response.set_ret(phxpf::PHXPARTITIONFILTER_OK);
    response.set_value(m_phxpfSm.FilterInstance()->FilterOne(op.args()));
    return phxpf::PHXPARTITIONFILTER_OK;
}

int PhxPartitionFilterNode::UpdateOne(const phxpf::PhxPFSingleOperator &op, phxpf::PhxPFSingleResponse &response) {
    if (m_paxosNode->IsIMMaster(GetGroupIdx())) {
        return PFPropose(op, response);
    }
    response.set_ret(phxpf::PHXPARTITIONFILTER_REDIRECT);
    return phxpf::PHXPARTITIONFILTER_NOTMASTER;
}

int PhxPartitionFilterNode::DeleteOne(const phxpf::PhxPFSingleOperator &op, phxpf::PhxPFSingleResponse &response) {
    if (m_paxosNode->IsIMMaster(GetGroupIdx())) {
        return PFPropose(op, response);
    }
    response.set_ret(phxpf::PHXPARTITIONFILTER_REDIRECT);
    return phxpf::PHXPARTITIONFILTER_NOTMASTER;
}

bool PhxPartitionFilterNode::Ready(const string *msg) const {
    phxpf::PhxPFMsg m;
    m.set_vtype(phxpf::PhxMsgType::READY);
    if (msg != nullptr) {
        m.set_value(*msg);
    }
    uint64_t llinstanceId = 0;
    int ret = m_paxosNode->Propose(0, m.SerializeAsString(), llinstanceId);
    return ret == 0;
}

bool PhxPartitionFilterNode::IsMaster() const {
    return m_paxosNode->IsIMMaster(0);
}
