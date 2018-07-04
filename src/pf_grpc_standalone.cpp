//
// Created by xianglizhao on 2018/6/27.
//

#include "pf_grpc_standalone.h"

PfGrpcStandaloneImpl::PfGrpcStandaloneImpl()
        : m_pfilter(nullptr) {

}

::grpc::Status PfGrpcStandaloneImpl::IsMaster(
        ::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::phxpf::PhxIAmMaster* response) {
    response->set_master(true);
    return grpc::Status::OK;
}

::grpc::Status PfGrpcStandaloneImpl::Filter(
        ::grpc::ServerContext* context, const ::phxpf::PhxPFOperator* request, ::phxpf::PhxPFResponse* response) {
    PartitionFilter::values_type contents(
            request->args().value().values().begin(), request->args().value().values().end());
    vector<string> res = m_pfilter->Filter(contents);
    *(response->mutable_value()->mutable_values()) = {res.begin(), res.end()};
    response->set_ret(phxpf::PHXPARTITIONFILTER_OK);
    return grpc::Status::OK;
}

::grpc::Status PfGrpcStandaloneImpl::Update(
        ::grpc::ServerContext* context, const ::phxpf::PhxPFOperator* request, ::phxpf::PhxPFResponse* response) {
    PartitionFilter::values_type contents(
            request->args().value().values().begin(), request->args().value().values().end());
    m_pfilter->Update(request->args().partition(), contents);
    response->set_ret(phxpf::PHXPARTITIONFILTER_OK);
    return grpc::Status::OK;
}

::grpc::Status PfGrpcStandaloneImpl::Delete(
        ::grpc::ServerContext* context, const ::phxpf::PhxPFOperator* request, ::phxpf::PhxPFResponse* response) {
    PartitionFilter::values_type contents(
            request->args().value().values().begin(), request->args().value().values().end());
    m_pfilter->Delete(contents);
    response->set_ret(phxpf::PHXPARTITIONFILTER_OK);
    return grpc::Status::OK;
}

::grpc::Status PfGrpcStandaloneImpl::FilterOne(
        ::grpc::ServerContext* context, const ::phxpf::PhxPFSingleOperator* request, ::phxpf::PhxPFSingleResponse* response) {
    response->set_value(m_pfilter->FilterOne(request->args().value()));
    response->set_ret(phxpf::PHXPARTITIONFILTER_OK);
    return grpc::Status::OK;
}

::grpc::Status PfGrpcStandaloneImpl::DeleteOne(
        ::grpc::ServerContext* context, const ::phxpf::PhxPFSingleOperator* request, ::phxpf::PhxPFSingleResponse* response) {
    m_pfilter->DeleteOne(request->args().value());
    response->set_ret(phxpf::PHXPARTITIONFILTER_OK);
    return grpc::Status::OK;
}

::grpc::Status PfGrpcStandaloneImpl::UpdateOne(
        ::grpc::ServerContext* context, const ::phxpf::PhxPFSingleOperator* request, ::phxpf::PhxPFSingleResponse* response) {
    m_pfilter->UpdateOne(request->args().partition(), request->args().value());
    response->set_ret(phxpf::PHXPARTITIONFILTER_OK);
    return grpc::Status::OK;
}

void PfGrpcStandaloneImpl::Init(const PartitionFilterConfig &config) {
    m_pfilter = new PartitionFilter(config);
}

PfGrpcStandaloneImpl::~PfGrpcStandaloneImpl() {
    if (m_pfilter != nullptr) {
        delete m_pfilter;
    }
}
