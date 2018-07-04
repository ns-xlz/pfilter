//
// Created by xianglizhao on 2018/5/24.
//

#ifndef PFILTER2_PHXPFILTER_H
#define PFILTER2_PHXPFILTER_H

#include <leveldb/db.h>
#include "pfilter.h"
#include "phxpfconfig.h"
#include "phxpf.pb.h"

namespace gpro = google::protobuf;

#define PHXPF_CHECKPOINT_KEY ((uint64_t)-1)

class PhxPartitionFilter {
private:
    boost::shared_mutex m_rwlock;
    PartitionFilterConfig m_config;
    leveldb::DB * m_poLevelDB;
    PartitionFilter * m_pfilter;
    bool m_bHasInit;

public:
    PhxPartitionFilter();
    bool Init(const PartitionFilterConfig & config);
    void Filter(const phxpf::PhxPFOperatorArgs & args, phxpf::PhxPFValue & response);
    void Update(const phxpf::PhxPFOperatorArgs & args, phxpf::PhxPFValue & response);
    void Delete(const phxpf::PhxPFOperatorArgs & args, phxpf::PhxPFValue & response);

    bool FilterOne(const phxpf::PhxPFSingleOperatorArgs & args);
    void UpdateOne(const phxpf::PhxPFSingleOperatorArgs & args);
    void DeleteOne(const phxpf::PhxPFSingleOperatorArgs & args);

    int GetCheckpointInstanceID(uint64_t &llCheckpointInstanceID);
    int SetCheckpointInstanceID(uint64_t llCheckpointInstanceID);

    ~PhxPartitionFilter();
};


#endif //PFILTER2_PHXPFILTER_H
