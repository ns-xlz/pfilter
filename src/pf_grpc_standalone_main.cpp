//
// Created by xianglizhao on 2018/6/27.
//

#include "pf_grpc_standalone.h"
#include <getopt.h>

void Usage(const char *bin) {
    std::cout << "usage: " << bin << " --config_file=/path/to/config.json [--daemon]" << std::endl;
    std::cout << "\t--bind_addr\t-b\trpc service listen addr, ip:port" << std::endl;
    std::cout << "\t--config_file\t-c\tjson format config file with absolute path" << std::endl;
    std::cout << "\t--daemon\t-d\trun as daemon" << std::endl;
    exit(-1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        Usage(argv[0]);
    }

    option options[] = {
            {"bind_addr", required_argument, 0, 'b'},
            {"config_file", required_argument, 0, 'c'},
            {"daemon",      optional_argument, 0, 'd'}
    };

    char *configFile{nullptr};
    int opt = 0;
    int longIndex = 0;
    bool runDaemon = false;
    char *bindAddr{nullptr};

    while ((opt = getopt_long(argc, argv, "b:c:d", options, &longIndex)) != -1) {
        switch (opt) {
            case 'b':
                bindAddr = optarg;
                break;
            case 'c':
                configFile = optarg;
                break;
            case 'd':
                runDaemon = true;
                break;
            default:
                Usage(argv[0]);
        }
    }

    if (configFile == nullptr) {
        Usage(argv[0]);
    }

    if (bindAddr == nullptr) {
        Usage(argv[0]);
    }

    PartitionFilterConfig config;
    if (!config.Read(configFile)) {
        std::cout << configFile << " can not be recognized" << std::endl;
        exit(-1);
    }

    config.Print();

    PfGrpcStandaloneImpl serverImpl;
    serverImpl.Init(config);

    ::grpc::ServerBuilder builder;
    builder.AddListeningPort(bindAddr, ::grpc::InsecureServerCredentials());
    builder.RegisterService(&serverImpl);

    std::unique_ptr<::grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << bindAddr << std::endl;

    if (runDaemon && (daemon(0, 0) == -1))
        std::cout << "daemon error" << std::endl;
    server->Wait();
}