//
// Created by xianglizhao on 2018/5/27.
//

#include "phxpfsm.h"
#include "phx_msg.h"
#include "log.h"

PhxPartitionFilterSm::PhxPartitionFilterSm()
    : m_llCheckpointInstanceID(phxpaxos::NoCheckpoint),
      m_skipSyncCheckpointTimes(0),
      m_phxpfilter(nullptr) {

}

bool PhxPartitionFilterSm::Init(const PartitionFilterConfig &config) {
    m_phxpfilter = new PhxPartitionFilter();
    bool succ = m_phxpfilter->Init(config);
    if (!succ) {
        return false;
    }

    int ret = m_phxpfilter->GetCheckpointInstanceID(m_llCheckpointInstanceID);
    if (ret != phxpf::PHXPARTITIONFILTER_OK) {
        if (ret == phxpf::PHXPARTITIONFILTER_KEY_NOT_EXISTS) {
            m_llCheckpointInstanceID = phxpaxos::NoCheckpoint;
        } else {
            PLErr("GetCheckpointInstanceID m_llCheckpointInstanceID failed: %d", ret);
            return false;
        }
    }
    return true;
}

bool PhxPartitionFilterSm::Execute(const int groupIdx, const uint64_t llInstanceID, const std::string &paxosValue,
                                   phxpaxos::SMCtx *poSMCtx) {
    phxpf::PhxPFMsg msg;
    if (!msg.ParseFromString(paxosValue)) {
        PLErr("ParseFromString failed: %s", paxosValue.c_str());
        return true;
    }

    if (msg.vtype() == phxpf::PhxMsgType::MULTI) {
        phxpf::PhxPFResponse response;
        phxpf::PhxPFResponse * res = poSMCtx != nullptr && poSMCtx->m_pCtx != nullptr ?
                                           (phxpf::PhxPFResponse *) poSMCtx->m_pCtx : &response;
        phxpf::PhxPFOperator request;
        if (request.ParseFromString(msg.value())) {
            bool ret = Excute(request, *res, llInstanceID);
            return ret;
        }
        PLErr("ParseFromString failed: %s", msg.value().c_str());
    } else if (msg.vtype() == phxpf::PhxMsgType::SINGLE) {
        phxpf::PhxPFSingleResponse response;
        phxpf::PhxPFSingleResponse * res = poSMCtx != nullptr && poSMCtx->m_pCtx != nullptr ?
            (phxpf::PhxPFSingleResponse *) poSMCtx->m_pCtx : &response;
        phxpf::PhxPFSingleOperator request;
        if (request.ParseFromString(msg.value())) {
            return Excute(request, *res, llInstanceID);
        }
        PLErr("ParseFromString failed: %s", msg.value().c_str());
    } else if (msg.vtype() == phxpf::PhxMsgType::READY) {
        // ready
        SyncCheckpointInstanceID(llInstanceID);
        return true;
    }
    PLErr("unknown msg type failed: %d", msg.vtype());
    return false;
}

bool PhxPartitionFilterSm::Excute(const phxpf::PhxPFOperator &op, phxpf::PhxPFResponse &response, const uint64_t llInstanceID) {
    response.set_ret(phxpf::PHXPARTITIONFILTER_OK);
    switch (op.operator_()) {
        case phxpf::PHXPF_FILTER:
            m_phxpfilter->Filter(op.args(), *response.mutable_value());
            return true;
        case phxpf::PHXPF_UPDATE:
            m_phxpfilter->Update(op.args(), *response.mutable_value());
            SyncCheckpointInstanceID(llInstanceID);
            return true;
        case phxpf::PHXPF_DELETE:
            m_phxpfilter->Delete(op.args(), *response.mutable_value());
            SyncCheckpointInstanceID(llInstanceID);
            return true;
        default:
            return false;
    }
}

bool PhxPartitionFilterSm::Excute(const phxpf::PhxPFSingleOperator &op, phxpf::PhxPFSingleResponse &response, const uint64_t llInstanceID) {
    response.set_ret(phxpf::PHXPARTITIONFILTER_OK);
    switch (op.operator_()) {
        case phxpf::PHXPF_SINGLE_FILTER:
            response.set_value(m_phxpfilter->FilterOne(op.args()));
            return true;
        case phxpf::PHXPF_SINGLE_UPDATE:
            m_phxpfilter->UpdateOne(op.args());
            SyncCheckpointInstanceID(llInstanceID);
            return true;
        case phxpf::PHXPF_SINGLE_DELETE:
            m_phxpfilter->DeleteOne(op.args());
            SyncCheckpointInstanceID(llInstanceID);
            return true;
        default:
            return false;
    }
}

PhxPartitionFilter* PhxPartitionFilterSm::FilterInstance() {
    return m_phxpfilter;
}

int PhxPartitionFilterSm::SyncCheckpointInstanceID(const uint64_t llInstanceID) {
    if (m_skipSyncCheckpointTimes++ < 5) {
        return 0;
    }

    int ret = this->m_phxpfilter->SetCheckpointInstanceID(llInstanceID);
    if (ret == phxpf::PHXPARTITIONFILTER_OK) {
        m_llCheckpointInstanceID = llInstanceID;
        m_skipSyncCheckpointTimes = 0;
    }
    return ret;
}

PhxPartitionFilterSm::~PhxPartitionFilterSm() {
    if (m_phxpfilter != nullptr) {
        delete m_phxpfilter;
    }
}
