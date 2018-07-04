CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
PROJECT(phxpf)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++11")

IF (NOT DEFINED ENV{PHXPAXOS_PATH})
    message(WARNING "if build phxpaxos server, there need environment variable: PHXPAXOS_PATH (phxpaxos root path), usage: PHXPAXOS_PATH=/path/to/phxpaxos cmake CMakeLists.txt")
    SET(PROTOC protoc)
ELSE ()
    SET(PHXPAXOS_PATH $ENV{PHXPAXOS_PATH})
    SET(PROTOC ${PHXPAXOS_PATH}/third_party/protobuf/bin/protoc)
ENDIF ()

IF (DEFINED ENV{BOOST_PATH})
    SET(BOOST_PATH $ENV{BOOST_PATH})
    SET(BOOST_CUSTOM_INCLUDE_PATH ${BOOST_PATH}/include)
    SET(BOOST_CUSTOM_LIBRARY_PATH ${BOOST_PATH}/lib)
ENDIF ()

IF (DEFINED ENV{GRPC_PATH})
    SET(GRPC_PATH $ENV{GRPC_PATH})
    SET(GRPC_CUSTOM_INCLUDE_PATH ${GRPC_PATH}/include)
    SET(GRPC_CUSTOM_LIBRARY_PATH ${GRPC_PATH}/lib)
ENDIF ()

INCLUDE_DIRECTORIES(${PHXPAXOS_PATH}/include
        ${PHXPAXOS_PATH}/plugin/include
        ${PHXPAXOS_PATH}/third_party/leveldb/include
        ${PHXPAXOS_PATH}/third_party/protobuf/include
        ${BOOST_CUSTOM_INCLUDE_PATH}
        ${GRPC_CUSTOM_INCLUDE_PATH}
        src)
LINK_DIRECTORIES(${PHXPAXOS_PATH}/lib
        ${PHXPAXOS_PATH}/third_party/leveldb/lib
        ${PHXPAXOS_PATH}/third_party/protobuf/lib
        ${BOOST_CUSTOM_LIBRARY_PATH}
        ${GRPC_CUSTOM_LIBRARY_PATH})

SET(CMAKE_CXX_STANDARD 11)
SET(PYTHON_DIR python)

SET(BLOOMFILTER_INCLUDE
        src/refbitset.h
        src/share_mapping.h
        src/bloomfilter.h)

SET(BLOOMFILTER_SOURCE_FILE
        src/refbitset.cpp
        src/share_mapping.cpp
        src/bloomfilter.cpp)

SET(PARTITION_BLOOMFILTER_INCLUDE
        src/common.h
        src/hash.h
        src/pfconfig.h
        src/pfilter.h)

SET(PARTITION_BLOOMFILTER_SOURCE_FILE
        src/hash.cpp
        src/pfconfig.cpp
        src/pfilter.cpp)

ADD_LIBRARY(pfilter
        STATIC
        ${BLOOMFILTER_SOURCE_FILE}
        ${PARTITION_BLOOMFILTER_SOURCE_FILE})

TARGET_LINK_LIBRARIES(pfilter
        boost_filesystem
        boost_system
        boost_date_time
        boost_thread)

ADD_CUSTOM_TARGET(cppproto
        ${PROTOC} -I ${CMAKE_CURRENT_LIST_DIR}/proto --grpc_out=${CMAKE_CURRENT_LIST_DIR}/src --cpp_out=${CMAKE_CURRENT_LIST_DIR}/src --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ${CMAKE_CURRENT_LIST_DIR}/proto/phxpf.proto)

ADD_CUSTOM_TARGET(pyproto
        ${PROTOC} -I ${CMAKE_CURRENT_LIST_DIR}/proto --grpc_out=${CMAKE_CURRENT_LIST_DIR}/src --python_out=${CMAKE_CURRENT_LIST_DIR}/src --plugin=protoc-gen-grpc=`which grpc_python_plugin` ${CMAKE_CURRENT_LIST_DIR}/proto/phxpf.proto)


SET(PHX_PFILTER_PROTO_INCLUDE
        ${CMAKE_CURRENT_LIST_DIR}/src/phxpf.pb.h
        ${CMAKE_CURRENT_LIST_DIR}/src/phxpf.grpc.pb.h)

SET(PHX_PFILTER_PROTO_SOURCE_FILE
        ${CMAKE_CURRENT_LIST_DIR}/src/phxpf.pb.cc
        ${CMAKE_CURRENT_LIST_DIR}/src/phxpf.grpc.pb.cc)

SET(PHX_PFILTER_PROTO_PYTHON_FILE
        ${CMAKE_CURRENT_LIST_DIR}/src/phxpf_pb2_grpc.py
        ${CMAKE_CURRENT_LIST_DIR}/src/phxpf_pb2.py)

SET_SOURCE_FILES_PROPERTIES(${PHX_PFILTER_PROTO_SOURCE_FILE}
        PROPERTIES GENERATED TRUE)

SET_SOURCE_FILES_PROPERTIES(${PHX_PFILTER_PROTO_PYTHON_FILE}
        PROPERTIES GENERATED TRUE)

SET(PHX_PFILTER_INCLUDE
        src/log.h
        src/phxpfconfig.h
        src/phxpfilter.h
        src/phxpfnode.h
        src/phxpfsm.h
        src/phx_msg.h)

SET(PHX_PFILTER_SOURCE_FILE
        src/log.cpp
        src/phxpfconfig.cpp
        src/phxpfilter.cpp
        src/phxpfnode.cpp
        src/phxpfsm.cpp
        src/phx_msg.cpp)

ADD_LIBRARY(phxpfilter STATIC
        ${PHX_PFILTER_PROTO_SOURCE_FILE}
        ${PHX_PFILTER_SOURCE_FILE})

ADD_DEPENDENCIES(phxpfilter cppproto)

TARGET_LINK_LIBRARIES(phxpfilter
        pfilter
        protobuf
        leveldb
        snappy
        phxpaxos
        phxpaxos_plugin
        glog
        pthread)

SET(PHX_PFILTER_GRPC_INCLUDE
        src/phxpf_grpc_server.h)

SET(PHX_PFILTER_GRPC_SOURCE_FILE
        src/phxpf_grpc_server.cpp
        src/phxpf_grpc_server_main.cpp)

SET(PF_PFILTER_GRPC_INCLUDE
        src/pf_grpc_standalone.h)

SET(PF_PFILTER_GRPC_SOURCE_FILE
        src/pf_grpc_standalone.cpp
        src/pf_grpc_standalone_main.cpp)

SET(PHX_PFILTER_GRPCTOOL_SOURCE_FILE
        src/phxpf_grpc_tool.cpp)

SET(PF_GRPCTOOL_SOURCE_FILE
        src/pf_grpc_tool.cpp)

SET(PHX_PFILTER_PYTHON_FILE
        ${CMAKE_CURRENT_LIST_DIR}/src/phxpf_grpc.py
        ${PHX_PFILTER_PROTO_PYTHON_FILE})

ADD_EXECUTABLE(phxpf_sample_server
        ${PHX_PFILTER_GRPC_SOURCE_FILE})

TARGET_LINK_LIBRARIES(phxpf_sample_server
        phxpfilter
        grpc++)

ADD_EXECUTABLE(pf_sample_standalone
        ${PF_PFILTER_GRPC_SOURCE_FILE}
        ${PHX_PFILTER_PROTO_SOURCE_FILE})

TARGET_LINK_LIBRARIES(pf_sample_standalone
        pfilter
        grpc++
        protobuf)

ADD_DEPENDENCIES(pf_sample_standalone
        cppproto)

ADD_EXECUTABLE(phxpf_sample_tool
        ${PHX_PFILTER_GRPCTOOL_SOURCE_FILE}
        ${PHX_PFILTER_PROTO_SOURCE_FILE})

ADD_EXECUTABLE(pf_sample_tool
        ${PF_GRPCTOOL_SOURCE_FILE}
        ${PHX_PFILTER_PROTO_SOURCE_FILE})

TARGET_LINK_LIBRARIES(phxpf_sample_tool
        phxpfilter
        grpc++)

TARGET_LINK_LIBRARIES(pf_sample_tool
        pfilter
        grpc++
        protobuf)

ADD_CUSTOM_TARGET(python
        mkdir -p ${PYTHON_DIR}
        COMMAND cp ${PHX_PFILTER_PYTHON_FILE} ${PYTHON_DIR}
        SOURCES ${PHX_PFILTER_PYTHON_FILE})

ADD_DEPENDENCIES(python pyproto)

SET_PROPERTY(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
        ${PHX_PFILTER_PROTO_INCLUDE}
        ${PHX_PFILTER_PROTO_SOURCE_FILE}
        ${PHX_PFILTER_PROTO_PYTHON_FILE})

SET(SAMPLE_TEST_FILE
        test/phxpf_grpc_test.py)

SET(SAMPLE_BUILD_CMAKE_FILE
        ${CMAKE_CURRENT_LIST_DIR}/sample.cmake)

ADD_CUSTOM_TARGET(sample_phxpfilter
        COMMAND ${CMAKE_COMMAND}
            -DSERVER_WORKDIR=${PROJECT_BINARY_DIR}/sample_phxpfilter
            -DSERVER_NODENUM=3
            -P ${CMAKE_CURRENT_LIST_DIR}/sample.cmake
        SOURCES ${SAMPLE_BUILD_CMAKE_FILE})

ADD_CUSTOM_TARGET(sample_standalone
        COMMAND ${CMAKE_COMMAND}
            -DSERVER_WORKDIR=${PROJECT_BINARY_DIR}/sample_standalone
            -DSERVER_NODENUM=3
            -P ${CMAKE_CURRENT_LIST_DIR}/sample.cmake
        SOURCES ${SAMPLE_BUILD_CMAKE_FILE})

INSTALL(TARGETS phxpf_sample_tool phxpf_sample_server phxpfilter pfilter
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

ENABLE_TESTING()
SET(SAMPLE_TEST_CMAKE_FILE ${CMAKE_CURRENT_LIST_DIR}/test/sampletest.cmake)

SET(SAMPLE_CMAKE_FILE ${CMAKE_CURRENT_LIST_DIR}/sample.cmake)
SET(SERVER_START_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/test/start_server.sh)
SET(SERVER_STOP_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/test/stop_server.sh)

SET(PHXPF_SERVER_WORKDIR ${PROJECT_BINARY_DIR}/sample_phxpfilter)
SET(PHXPF_SERVER_BINARY ${PROJECT_BINARY_DIR}/phxpf_sample_server)
SET(PHXPF_SERVER_TOOL ${PROJECT_BINARY_DIR}/phxpf_sample_tool)
SET(PHXPF_SERVER_NODENUM 3)
SET(PHXPF_SERVER_ADDR "127.0.0.1:11261,127.0.0.1:11262,127.0.0.1:11263")

SET(PF_SERVER_WORKDIR ${PROJECT_BINARY_DIR}/sample_standalone)
SET(PF_SERVER_BINARY ${PROJECT_BINARY_DIR}/pf_sample_standalone)
SET(PF_SERVER_TOOL ${PROJECT_BINARY_DIR}/pf_sample_tool)
SET(PF_SERVER_NODENUM 1)
SET(PF_SERVER_ADDR "127.0.0.1:11264")

SET(TEST_TOOLS_FILES
        ${CMAKE_CURRENT_LIST_DIR}/test/pftesttool.py
        ${CMAKE_CURRENT_LIST_DIR}/test/start_server.sh
        ${CMAKE_CURRENT_LIST_DIR}/test/stop_server.sh)

SET(STANDALONE_PYTEST_FILE ${CMAKE_CURRENT_LIST_DIR}/test/pf_test.py)
SET(PHXPFILTER_PYTEST_FILE ${CMAKE_CURRENT_LIST_DIR}/test/phxpf_test.py)
SET(STANDALONE_BENCHMARK_FILE ${CMAKE_CURRENT_LIST_DIR}/test/pf_benchmark.py)
SET(PHXPFILTER_BENCHMARK_FILE ${CMAKE_CURRENT_LIST_DIR}/test/phxpf_benchmark.py)

ADD_SUBDIRECTORY(test)