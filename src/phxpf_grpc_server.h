//
// Created by xianglizhao on 2018/6/1.
//

#ifndef PFILTER2_PHXPF_GRPC_SERVER_H
#define PFILTER2_PHXPF_GRPC_SERVER_H

#include <grpc++/grpc++.h>
#include "phxpfnode.h"
#include "phxpf.grpc.pb.h"

class PhxPFGrpcServer final : public phxpf::PhxPFServer::Service {
private:
    PhxPartitionFilterNode m_node;
public:
    PhxPFGrpcServer();
    int Init(const PhxPartitionFilterNodeConfig &config);
    bool Ready() const;

    virtual ::grpc::Status IsMaster(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::phxpf::PhxIAmMaster* response);
    virtual ::grpc::Status Filter(::grpc::ServerContext* context, const ::phxpf::PhxPFOperator* request, ::phxpf::PhxPFResponse* response);
    virtual ::grpc::Status Update(::grpc::ServerContext* context, const ::phxpf::PhxPFOperator* request, ::phxpf::PhxPFResponse* response);
    virtual ::grpc::Status Delete(::grpc::ServerContext* context, const ::phxpf::PhxPFOperator* request, ::phxpf::PhxPFResponse* response);
    virtual ::grpc::Status FilterOne(::grpc::ServerContext* context, const ::phxpf::PhxPFSingleOperator* request, ::phxpf::PhxPFSingleResponse* response);
    virtual ::grpc::Status UpdateOne(::grpc::ServerContext* context, const ::phxpf::PhxPFSingleOperator* request, ::phxpf::PhxPFSingleResponse* response);
    virtual ::grpc::Status DeleteOne(::grpc::ServerContext* context, const ::phxpf::PhxPFSingleOperator* request, ::phxpf::PhxPFSingleResponse* response);
};


#endif //PFILTER2_PHXPF_GRPC_SERVER_H
