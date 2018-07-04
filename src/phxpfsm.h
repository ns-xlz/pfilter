//
// Created by xianglizhao on 2018/5/27.
//

#ifndef PFILTER2_PHXPFSM_H
#define PFILTER2_PHXPFSM_H

#include <phxpaxos/sm.h>
#include "phxpfilter.h"

class PhxPartitionFilterSm : public phxpaxos::StateMachine {
private:
    PhxPartitionFilter * m_phxpfilter;
    uint64_t m_llCheckpointInstanceID;
    int m_skipSyncCheckpointTimes;

public:
    PhxPartitionFilterSm();
    ~PhxPartitionFilterSm();
    int SyncCheckpointInstanceID(const uint64_t llInstanceID);
    bool Execute(const int iGroupIdx, const uint64_t llInstanceID,
                 const std::string &sPaxosValue, phxpaxos::SMCtx *poSMCtx);

    PhxPartitionFilter * FilterInstance();

    bool Excute(const phxpf::PhxPFOperator &op, phxpf::PhxPFResponse & response, const uint64_t llInstanceID);
    bool Excute(const phxpf::PhxPFSingleOperator &op, phxpf::PhxPFSingleResponse & response, const uint64_t llInstanceID);

    bool Init(const PartitionFilterConfig & config);

public:
    int LockCheckpointState() {return -1;}
    int GetCheckpointState(const int iGroupIdx, std::string &sDirPath,
                           std::vector<std::string> &vecFileList) { return -1; }
    void UnLockCheckpointState() {}
    int LoadCheckpointState(const int iGroupIdx, const std::string &sCheckpointTmpFileDirPath,
                            const std::vector<std::string> &vecFileList,
                            const uint64_t llCheckpointInstanceID) { return -1; }

    const int SMID() const { return 1; }

    bool ExecuteForCheckpoint(const int iGroupIdx, const uint64_t llInstanceID,
                              const std::string &sPaxosValue) { return true; }
    const uint64_t GetCheckpointInstanceID(const int iGroupIdx) const { return m_llCheckpointInstanceID; }
};


#endif //PFILTER2_PHXPFSM_H
