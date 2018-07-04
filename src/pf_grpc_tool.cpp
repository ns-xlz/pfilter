//
// Created by xianglizhao on 2018/6/4.
//

#include <grpc++/grpc++.h>
#include "phxpf.grpc.pb.h"
#include <boost/algorithm/string.hpp>
#include <getopt.h>
#include <google/protobuf/empty.pb.h>
#include <exception>

using namespace std;

std::unique_ptr<phxpf::PhxPFServer::Stub> NewChannel(const char * serverAddr) {
    std::unique_ptr<phxpf::PhxPFServer::Stub> stub =
            phxpf::PhxPFServer::NewStub(grpc::CreateChannel(serverAddr, grpc::InsecureChannelCredentials()));
    return stub;
}

void Usage(const char * bin) {
    std::cout<<"usage: "<<bin<<" -s <server addresses,> -f <func> -p <partition> -v <value,value>"<<std::endl;
    std::cout<<"\t"<<"--server\t-s\tserver addresses ip:port"<<std::endl;
    std::cout<<"\t"<<"--func\t-f\trpc function name [filter, filterOne, update, updateOne, delete, deleteOne]"<<std::endl;
    std::cout<<"\t"<<"--partition\t-p\tpf partition, when use update function, must point"<<std::endl;
    std::cout<<"\t"<<"--value\t-v\trpc pf server function params data"<<std::endl;
    exit(-1);
}

void Filter(std::unique_ptr<phxpf::PhxPFServer::Stub> stub,
            const char * partition,
            const char * value) {
    std::vector<string> values;
    boost::split(values, value, boost::is_any_of(","), boost::token_compress_on);
    if (values.size() < 1) {
        std::cout<<"value size is too small"<<std::endl;
        Usage("phxpf");
    }

    phxpf::PhxPFOperator op;
    phxpf::PhxPFResponse response;
    grpc::ClientContext context;
    op.set_operator_(phxpf::PHXPF_FILTER);
    phxpf::PhxPFOperatorArgs * args = op.mutable_args();
    args->set_partition(partition);
    for (auto v : values) {
        args->mutable_value()->add_values(v);
    }

    grpc::Status status = stub->Filter(&context, op, &response);
    if (!status.ok()) {
        std::cout<<"some error, code: "<<status.error_code()<<std::endl;
    } else {
        std::cout<<"succeed"<<std::endl;
    }
    if (response.ret() != phxpf::PHXPARTITIONFILTER_OK) {
        std::cout<<"pfilter happend some error, code: "<< response.ret()<<std::endl;
    }

    for (auto i : response.value().values()) {
        std::cout<<i<<std::endl;
    }
}

void Update(std::unique_ptr<phxpf::PhxPFServer::Stub> stub,
            const char * partition,
            const char * value) {
    std::vector<string> values;
    boost::split(values, value, boost::is_any_of(","), boost::token_compress_on);
    if (values.size() < 1) {
        std::cout<<"value size is too small"<<std::endl;
        Usage("phxpf");
    }

    phxpf::PhxPFOperator op;
    phxpf::PhxPFResponse response;
    grpc::ClientContext context;
    op.set_operator_(phxpf::PHXPF_UPDATE);
    phxpf::PhxPFOperatorArgs * args = op.mutable_args();
    args->set_partition(partition);
    for (auto v : values) {
        args->mutable_value()->add_values(v);
    }

    grpc::Status status = stub->Update(&context, op, &response);
    if (!status.ok()) {
        std::cout<<"some error, code: "<<status.error_code()<<std::endl;
    } else {
        std::cout<<"succeed"<<std::endl;
    }
    if (response.ret() != phxpf::PHXPARTITIONFILTER_OK) {
        std::cout<<"pfilter happend some error, code: "<< response.ret()<<std::endl;
    }

    for (auto i : response.value().values()) {
        std::cout<<i<<std::endl;
    }
}

void Delete(std::unique_ptr<phxpf::PhxPFServer::Stub> stub,
            const char * partition,
            const char * value) {
    std::vector<string> values;
    boost::split(values, value, boost::is_any_of(","), boost::token_compress_on);
    if (values.size() < 1) {
        std::cout<<"value size is too small"<<std::endl;
        Usage("phxpf");
    }

    phxpf::PhxPFOperator op;
    phxpf::PhxPFResponse response;
    grpc::ClientContext context;
    op.set_operator_(phxpf::PHXPF_DELETE);
    phxpf::PhxPFOperatorArgs * args = op.mutable_args();
    args->set_partition(partition);
    for (auto v : values) {
        args->mutable_value()->add_values(v);
    }

    grpc::Status status = stub->Delete(&context, op, &response);
    if (!status.ok()) {
        std::cout<<"some error, code: "<<status.error_code()<<std::endl;
    } else {
        std::cout<<"succeed"<<std::endl;
    }
    if (response.ret() != phxpf::PHXPARTITIONFILTER_OK) {
        std::cout<<"pfilter happend some error, code: "<< response.ret()<<std::endl;
    }

    for (auto i : response.value().values()) {
        std::cout<<i<<std::endl;
    }
}

void FilterOne(std::unique_ptr<phxpf::PhxPFServer::Stub> stub,
               const char * partition,
               const char * value) {
    phxpf::PhxPFSingleOperator op;
    phxpf::PhxPFSingleResponse response;
    grpc::ClientContext context;
    op.set_operator_(phxpf::PHXPF_SINGLE_FILTER);
    phxpf::PhxPFSingleOperatorArgs * args = op.mutable_args();
    args->set_partition(partition);
    args->set_value(value);

    grpc::Status status = stub->FilterOne(&context, op, &response);
    if (!status.ok()) {
        std::cout<<"some error, code: "<<status.error_code()<<std::endl;
    } else {
        std::cout<<"succeed"<<std::endl;
    }
    if (response.ret() != phxpf::PHXPARTITIONFILTER_OK) {
        std::cout<<"pfilter happend some error, code: "<< response.ret()<<std::endl;
    }
    std::cout<<response.value()<<endl;
}

void UpdateOne(std::unique_ptr<phxpf::PhxPFServer::Stub> stub,
               const char * partition,
               const char * value) {
    phxpf::PhxPFSingleOperator op;
    phxpf::PhxPFSingleResponse response;
    grpc::ClientContext context;
    op.set_operator_(phxpf::PHXPF_SINGLE_UPDATE);
    phxpf::PhxPFSingleOperatorArgs * args = op.mutable_args();
    args->set_partition(partition);
    args->set_value(value);

    grpc::Status status = stub->UpdateOne(&context, op, &response);
    if (!status.ok()) {
        std::cout<<"some error, code: "<<status.error_code()<<std::endl;
    } else {
        std::cout<<"succeed"<<std::endl;
    }
    if (response.ret() != phxpf::PHXPARTITIONFILTER_OK) {
        std::cout<<"pfilter happend some error, code: "<< response.ret()<<std::endl;
    }
}

void DeleteOne(std::unique_ptr<phxpf::PhxPFServer::Stub> stub,
               const char * partition,
               const char * value) {
    phxpf::PhxPFSingleOperator op;
    phxpf::PhxPFSingleResponse response;
    grpc::ClientContext context;
    op.set_operator_(phxpf::PHXPF_SINGLE_DELETE);
    phxpf::PhxPFSingleOperatorArgs * args = op.mutable_args();
    args->set_partition(partition);
    args->set_value(value);

    grpc::Status status = stub->DeleteOne(&context, op, &response);
    if (!status.ok()) {
        std::cout<<"some error, code: "<<status.error_code()<<std::endl;
    } else {
        std::cout<<"succeed"<<std::endl;
    }
    if (response.ret() != phxpf::PHXPARTITIONFILTER_OK) {
        std::cout<<"pfilter happend some error, code: "<< response.ret()<<std::endl;
    }
}

void RpcCallPF(const char * serverAddr,
               const char * func,
               const char * partition,
               const char * value) {
    if (strcasecmp(func, "filter") == 0) {
        Filter(NewChannel(serverAddr), partition, value);
    } else if (strcasecmp(func, "update") == 0) {
        Update(NewChannel(serverAddr), partition, value);
    } else if (strcasecmp(func, "delete") == 0) {
        Delete(NewChannel(serverAddr), partition, value);
    } else if (strcasecmp(func, "filterone") == 0) {
        FilterOne(NewChannel(serverAddr), partition, value);
    } else if (strcasecmp(func, "updateone") == 0) {
        UpdateOne(NewChannel(serverAddr), partition, value);
    } else if (strcasecmp(func, "deleteone") == 0) {
        DeleteOne(NewChannel(serverAddr), partition, value);
    } else {
        std::cout<<"only support: filter, filterOne, update, updateOne, delete, deleteOne"<<std::endl;
    }
}

int main(int argc, char * argv[]) {
    option options[] = {
            {"server", required_argument, 0, 's' },
            {"func", required_argument, 0, 'f'},
            {"partition", optional_argument, 0, 'p'},
            {"value", required_argument, 0, 'v'},
    };

    char * serverAddr{nullptr};
    char * func{nullptr};
    char * partition{nullptr};
    char * value{nullptr};

    int opt = 0;
    int longIndex = 0;
    while((opt = getopt_long(argc, argv, "s:f:p:v:", options, &longIndex)) != -1) {
        switch (opt) {
            case 's':
                serverAddr = optarg;
                break;
            case 'f':
                func = optarg;
                break;
            case 'p':
                partition = optarg;
                break;
            case 'v':
                value = optarg;
                break;
            default:
                Usage(argv[0]);
        }
    }

    if (serverAddr == nullptr || func == nullptr || value == nullptr) {
        Usage(argv[0]);
    }

    if (strcasecmp(func, "update") == 0 && partition == nullptr) {
        Usage(argv[0]);
    }

    RpcCallPF(serverAddr, func, partition, value);

    return 0;
}