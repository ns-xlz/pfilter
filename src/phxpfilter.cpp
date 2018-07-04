//
// Created by xianglizhao on 2018/5/24.
//

#include "phxpfilter.h"
#include "log.h"

PhxPartitionFilter::PhxPartitionFilter()
        : m_bHasInit(false), m_pfilter(nullptr) {

}

bool PhxPartitionFilter::Init(const PartitionFilterConfig &config) {
    if (m_bHasInit) {
        return true;
    }

    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, config.InstancePath(), &m_poLevelDB);
    if (!status.ok()) {
        std::cout<<"instance db open: "<<config.InstancePath()<<" failed: "<<status.ToString()<<std::endl;
        return false;
    }

    m_pfilter = new PartitionFilter(config);
    m_bHasInit = true;
    return true;
}

void PhxPartitionFilter::Filter(const phxpf::PhxPFOperatorArgs & args, phxpf::PhxPFValue & response) {
    if (!m_bHasInit) {
        return;
    }

    PartitionFilter::values_type contents(args.value().values().begin(), args.value().values().end());
    vector<string> res = m_pfilter->Filter(contents);
    *response.mutable_values() = {res.begin(), res.end()};
}

void PhxPartitionFilter::Update(const phxpf::PhxPFOperatorArgs & args, phxpf::PhxPFValue & response) {
    if (!m_bHasInit) {
        return;
    }

    PartitionFilter::values_type contents(args.value().values().begin(), args.value().values().end());
    m_pfilter->Update(args.partition(), contents);
}

void PhxPartitionFilter::Delete(const phxpf::PhxPFOperatorArgs & args, phxpf::PhxPFValue & response) {
    if (!m_bHasInit) {
        return;
    }

    PartitionFilter::values_type contents(args.value().values().begin(), args.value().values().end());
    m_pfilter->Delete(contents);
}

bool PhxPartitionFilter::FilterOne(const phxpf::PhxPFSingleOperatorArgs & args) {
    if (!m_bHasInit) {
        return false;
    }

    return m_pfilter->FilterOne(args.value());
}

void PhxPartitionFilter::UpdateOne(const phxpf::PhxPFSingleOperatorArgs & args) {
    m_pfilter->UpdateOne(args.partition(), args.value());
}

void PhxPartitionFilter::DeleteOne(const phxpf::PhxPFSingleOperatorArgs & args) {
    if (!m_bHasInit) {
        return;
    }

    m_pfilter->DeleteOne(args.value());
}

int PhxPartitionFilter::GetCheckpointInstanceID(uint64_t &llCheckpointInstanceID) {
    if (!m_bHasInit) {
        return phxpf::PHXPARTITIONFILTER_SYS_FAIL;
    }

    string key;
    static uint64_t llCheckpointInstanceIDKey = PHXPF_CHECKPOINT_KEY;
    key.append((char*)&llCheckpointInstanceIDKey, sizeof(uint64_t));

    string buffer;
    leveldb::Status status = m_poLevelDB->Get(leveldb::ReadOptions(), key, &buffer);
    if (!status.ok()) {
        if (status.IsNotFound()) {
            PLErr("not found key : %s", key.c_str());
            return phxpf::PHXPARTITIONFILTER_KEY_NOT_EXISTS;
        }
        PLErr("happend some error for get key: %s, %s", key.c_str(), status.ToString().c_str());
        return phxpf::PHXPARTITIONFILTER_SYS_FAIL;
    }

    memcpy(&llCheckpointInstanceID, buffer.data(), sizeof(uint64_t));
    return phxpf::PHXPARTITIONFILTER_OK;
}

int PhxPartitionFilter::SetCheckpointInstanceID(uint64_t llCheckpointInstanceID) {
    if (!m_bHasInit) {
        return phxpf::PHXPARTITIONFILTER_SYS_FAIL;
    }

    std::string key;
    static uint64_t llCheckpointInstanceIDKey = PHXPF_CHECKPOINT_KEY;
    key.append((char *) &llCheckpointInstanceIDKey, sizeof(uint64_t));

    std::string buffer;
    buffer.append((char *) &llCheckpointInstanceID, sizeof(uint64_t));

    leveldb::WriteOptions oWriteOptions;
    //must fync
    oWriteOptions.sync = true;

    leveldb::Status status = m_poLevelDB->Put(oWriteOptions, key, buffer);
    if (!status.ok()) {
        PLErr("SetCheckpointInstanceID: %s failed", key.c_str());
        return phxpf::PHXPARTITIONFILTER_SYS_FAIL;
    }

    return phxpf::PHXPARTITIONFILTER_OK;
}

PhxPartitionFilter::~PhxPartitionFilter() {
    if (m_pfilter != nullptr) {
        delete m_pfilter;
    }
}