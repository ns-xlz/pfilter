//
// Created by xianglizhao on 2018/6/1.
//

#include "phxpf_grpc_server.h"

const grpc::Status RETRY(grpc::StatusCode::ABORTED, "need retry");

PhxPFGrpcServer::PhxPFGrpcServer() {

}

int PhxPFGrpcServer::Init(const PhxPartitionFilterNodeConfig &config) {
    return m_node.RunPaxos(config);
}

bool PhxPFGrpcServer::Ready() const {
    return m_node.Ready(nullptr);
}

::grpc::Status PhxPFGrpcServer::IsMaster(
        ::grpc::ServerContext *context,
        const ::google::protobuf::Empty *request,
        ::phxpf::PhxIAmMaster *response) {
    response->set_master(m_node.IsMaster());
    return ::grpc::Status::OK;
}

::grpc::Status PhxPFGrpcServer::Filter(::grpc::ServerContext *context, const ::phxpf::PhxPFOperator *request,
                                       ::phxpf::PhxPFResponse *response) {
    int ret = m_node.Filter(*request, *response);
    if (ret != 0) {
        PLErr("request filter failed with ret: %d", ret);
        return RETRY;
    }
    return ::grpc::Status::OK;
}

::grpc::Status PhxPFGrpcServer::Update(::grpc::ServerContext *context, const ::phxpf::PhxPFOperator *request,
                                       ::phxpf::PhxPFResponse *response) {
    int ret = m_node.Update(*request, *response);
    if (ret != 0) {
        PLErr("request update failed with ret: %d", ret);
        return RETRY;
    }
    return ::grpc::Status::OK;
}

::grpc::Status PhxPFGrpcServer::Delete(::grpc::ServerContext *context, const ::phxpf::PhxPFOperator *request,
                                       ::phxpf::PhxPFResponse *response) {
    int ret = m_node.Delete(*request, *response);
    if (ret != 0) {
        PLErr("request delete failed with ret: %d", ret);
        return RETRY;
    }
    return ::grpc::Status::OK;
}

::grpc::Status PhxPFGrpcServer::FilterOne(::grpc::ServerContext *context, const ::phxpf::PhxPFSingleOperator *request,
                                          ::phxpf::PhxPFSingleResponse *response) {
    int ret = m_node.FilterOne(*request, *response);
    if (ret != 0) {
        PLErr("request filterone failed with ret: %d", ret);
        return RETRY;
    }
    return ::grpc::Status::OK;
}

::grpc::Status PhxPFGrpcServer::UpdateOne(::grpc::ServerContext *context, const ::phxpf::PhxPFSingleOperator *request,
                                          ::phxpf::PhxPFSingleResponse *response) {
    int ret = m_node.UpdateOne(*request, *response);
    if (ret != 0) {
        PLErr("request updateone failed with ret: %d", ret);
        return RETRY;
    }
    return ::grpc::Status::OK;
}

::grpc::Status PhxPFGrpcServer::DeleteOne(::grpc::ServerContext *context, const ::phxpf::PhxPFSingleOperator *request,
                                          ::phxpf::PhxPFSingleResponse *response) {
    int ret = m_node.DeleteOne(*request, *response);
    if (ret != 0) {
        PLErr("request deleteone failed with ret: %d", ret);
        return RETRY;
    }
    return ::grpc::Status::OK;
}