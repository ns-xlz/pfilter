//
// Created by xianglizhao on 2018/6/27.
//

#ifndef PF_PHXPF_GRPC_STANDALONE_H
#define PF_PHXPF_GRPC_STANDALONE_H

#include <grpc++/grpc++.h>
#include "pfilter.h"
#include "phxpf.grpc.pb.h"

class PfGrpcStandaloneImpl final : public phxpf::PhxPFServer::Service {
private:
    PartitionFilter * m_pfilter;

public:
    PfGrpcStandaloneImpl();
    ~PfGrpcStandaloneImpl();
    void Init(const PartitionFilterConfig &config);

    virtual ::grpc::Status IsMaster(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::phxpf::PhxIAmMaster* response);
    virtual ::grpc::Status Filter(::grpc::ServerContext* context, const ::phxpf::PhxPFOperator* request, ::phxpf::PhxPFResponse* response);
    virtual ::grpc::Status Update(::grpc::ServerContext* context, const ::phxpf::PhxPFOperator* request, ::phxpf::PhxPFResponse* response);
    virtual ::grpc::Status Delete(::grpc::ServerContext* context, const ::phxpf::PhxPFOperator* request, ::phxpf::PhxPFResponse* response);
    virtual ::grpc::Status FilterOne(::grpc::ServerContext* context, const ::phxpf::PhxPFSingleOperator* request, ::phxpf::PhxPFSingleResponse* response);
    virtual ::grpc::Status UpdateOne(::grpc::ServerContext* context, const ::phxpf::PhxPFSingleOperator* request, ::phxpf::PhxPFSingleResponse* response);
    virtual ::grpc::Status DeleteOne(::grpc::ServerContext* context, const ::phxpf::PhxPFSingleOperator* request, ::phxpf::PhxPFSingleResponse* response);
};


#endif //PHXPF_PHXPF_GRPC_STANDALONE_H
