#!/usr/bin/env python
# coding: utf-8

import random
import grpc
from google.protobuf.empty_pb2 import Empty

from phxpf_pb2 import (
    PhxPFOperator, PhxPFSingleOperator, PHXPF_FILTER, PHXPF_UPDATE,
    PHXPF_DELETE, PHXPF_SINGLE_FILTER,
    PHXPF_SINGLE_UPDATE, PHXPF_SINGLE_DELETE, PHXPARTITIONFILTER_OK,
    PHXPARTITIONFILTER_CLIENT_RETRY, PHXPARTITIONFILTER_NOTMASTER)
from phxpf_pb2_grpc import PhxPFServerStub


class PfGrpcException(Exception):
    pass


class PfGrpcParamTypeError(PfGrpcException):
    pass


class PfGrpcReturnError(PfGrpcException):
    pass


connection_options = [
    ("grpc.max_connection_idle_ms", 3000),
    ("grpc.keepalive_timeout_ms", 5000),
    ("grpc.keepalive_permit_without_calls", 1),
    ("grpc.min_reconnect_backoff_ms", 100),
    ("grpc.max_reconnect_backoff_ms", 200),
    ("grpc.initial_reconnect_backoff_ms", 100),
    ("grpc.server_handshake_timeout_ms", 300),
    ("grpc.enable_retries", 1),
    ("grpc.http2.min_time_between_pings_ms", 1000),
    ("grpc.http2.min_ping_interval_without_data_ms", 2000),
]


class PhxPfGrpcClient(object):
    def __init__(self, rpc_addrs, retry=0, options=None, use_master=True):
        self.extra_option = dict(connection_options)
        if options and isinstance(options, dict):
            self.extra_option.update(options)
        self.rpc_addrs = rpc_addrs if len(rpc_addrs) > 1 else [
            rpc_addrs[0], rpc_addrs[0]]
        self.retry = retry
        self.__master = None
        self.__slaves = []
        self.user_master = use_master

    def __check_master_slave(self):
        if not self.__master or not self.__slaves:
            self.__slaves = []
            for host, port in self.rpc_addrs:
                try:
                    stub = PhxPFServerStub(
                        grpc.insecure_channel('%s:%s' % (host, port),
                                              options=self.extra_option.items()))
                    ret = stub.IsMaster(Empty())
                    if ret.master:
                        self.__master = stub
                        if self.user_master:
                            self.__slaves = [stub]
                            break
                    else:
                        if not self.user_master:
                            self.__slaves.append(stub)
                except Exception:
                    raise PfGrpcException(
                        "%s:%s check master slave failed" % (host, port))

            if not self.__master:
                raise PfGrpcException("no master")
            if not self.__slaves:
                self.__slaves = [self.__master]

    def __rpc_with_op(self, optype, partition, contents, id=None, retry=0):
        self.__check_master_slave()
        if not isinstance(contents, list):
            raise PfGrpcParamTypeError("contents must be string or int list")
        if len(contents) == 0:
            return contents

        contentints = False
        if isinstance(contents[0], int):
            contentints = True
        elif not isinstance(contents[0], str):
            raise PfGrpcParamTypeError("contents must be string or int list")

        idcard = None
        try:
            idcard = str(id)
        except Exception:
            raise PfGrpcParamTypeError("param id must can be stringfy")

        ics = contents
        if idcard:
            ics = ["%s:%s" % (idcard, c) for c in contents]
        op = PhxPFOperator()
        op.operator = optype
        op.args.partition = str(partition)
        op.args.value.values.extend(ics)

        if optype == PHXPF_FILTER:
            res = random.choice(self.__slaves).Filter(op)
        if optype == PHXPF_UPDATE:
            res = self.__master.Update(op)
        if optype == PHXPF_DELETE:
            res = self.__master.Delete(op)

        if res.ret == PHXPARTITIONFILTER_NOTMASTER and retry > 0:
            return self.__rpc_with_op(optype, partition, contents, retry - 1)

        if res.ret == PHXPARTITIONFILTER_CLIENT_RETRY and retry > 0:
            return self.__rpc_with_op(optype, partition, contents, retry - 1)

        if res.ret != PHXPARTITIONFILTER_OK:
            raise PfGrpcReturnError(res.msg)
        if not idcard:
            return map(lambda x: int(x), res.value.values[:]) if \
                contentints else res.value.values[:]
        else:
            return map(lambda x: int(x.split(":")[1]), res.value.values[:]) if \
                contentints else map(lambda x: x.split(":")[1],
                                     res.value.values[:])

    def __rpc_with_single_op(self, optype, partition, content, id=None,
                             retry=0):
        self.__check_master_slave()
        idcard = None
        try:
            idcard = str(id)
        except Exception:
            raise PfGrpcParamTypeError("param id must can be stringfy")

        if not isinstance(content, str) and not isinstance(content, int):
            raise PfGrpcParamTypeError("content must be string or int")

        ic = content
        if idcard:
            ic = "%s:%s" % (idcard, content)
        op = PhxPFSingleOperator()
        op.operator = optype
        op.args.partition = str(partition)
        op.args.value = ic

        if optype == PHXPF_SINGLE_FILTER:
            res = random.choice(self.__slaves).FilterOne(op)
        if optype == PHXPF_SINGLE_UPDATE:
            res = self.__master.UpdateOne(op)
        if optype == PHXPF_SINGLE_DELETE:
            res = self.__master.DeleteOne(op)

        if res.ret == PHXPARTITIONFILTER_NOTMASTER and retry > 0:
            return self.__rpc_with_single_op(optype, partition, content,
                                             retry - 1)

        if res.ret == PHXPARTITIONFILTER_CLIENT_RETRY and retry > 0:
            return self.__rpc_with_single_op(optype, partition, content,
                                             retry - 1)

        if res.ret != PHXPARTITIONFILTER_OK:
            raise PfGrpcReturnError(res.msg)

        return res.value

    def filter(self, partition, contents, id=None):
        return self.__rpc_with_op(
            PHXPF_FILTER, partition, contents, id, self.retry)

    def update(self, partition, contents, id=None):
        return self.__rpc_with_op(
            PHXPF_UPDATE, partition, contents, id, self.retry)

    def delete(self, partition, contents, id=None):
        return self.__rpc_with_op(
            PHXPF_DELETE, partition, contents, id, self.retry)

    def filterone(self, partition, content, id=None):
        return self.__rpc_with_single_op(
            PHXPF_SINGLE_FILTER, partition, content, id, self.retry)

    def updateone(self, partition, content, id=None):
        return self.__rpc_with_single_op(
            PHXPF_SINGLE_UPDATE, partition, content, id, self.retry)

    def deleteone(self, partition, content, id=None):
        return self.__rpc_with_single_op(
            PHXPF_SINGLE_DELETE, partition, content, id, self.retry)


class PfGrpcStandaloneClient(object):
    def __init__(self, rpc_addr, retry=0, options=None):
        self.extra_option = dict(connection_options)
        if options and isinstance(options, dict):
            self.extra_option.update(options)
        self.rpc_addr = rpc_addr
        self.retry = retry
        self.__stub = self.__new_stub()

    def __new_stub(self):
        stub = PhxPFServerStub(
            grpc.insecure_channel(
                '%s:%s' % (
                    self.rpc_addr[0], self.rpc_addr[1]),
                options=self.extra_option.items()))
        return stub

    def __rpc_with_op(self, optype, partition, contents, id=None, retry=0):
        if not isinstance(contents, list):
            raise PfGrpcParamTypeError("contents must be string or int list")
        if len(contents) == 0:
            return contents

        contentints = False
        if isinstance(contents[0], int):
            contentints = True
        elif not isinstance(contents[0], str):
            raise PfGrpcParamTypeError("contents must be string or int list")

        idcard = None
        try:
            idcard = str(id)
        except Exception:
            raise PfGrpcParamTypeError("param id must can be stringfy")

        ics = contents
        if idcard:
            ics = ["%s:%s" % (idcard, c) for c in contents]
        op = PhxPFOperator()
        op.operator = optype
        op.args.partition = str(partition)
        op.args.value.values.extend(ics)

        if optype == PHXPF_FILTER:
            res = self.__stub.Filter(op)
        if optype == PHXPF_UPDATE:
            res = self.__stub.Update(op)
        if optype == PHXPF_DELETE:
            res = self.__stub.Delete(op)

        if res.ret == PHXPARTITIONFILTER_NOTMASTER and retry > 0:
            return self.__rpc_with_op(optype, partition, contents, retry - 1)

        if res.ret == PHXPARTITIONFILTER_CLIENT_RETRY and retry > 0:
            return self.__rpc_with_op(optype, partition, contents, retry - 1)

        if res.ret != PHXPARTITIONFILTER_OK:
            raise PfGrpcReturnError(res.msg)
        if not idcard:
            return map(lambda x: int(x), res.value.values[:]) if \
                contentints else res.value.values[:]
        else:
            return map(lambda x: int(x.split(":")[1]), res.value.values[:]) if \
                contentints else map(lambda x: x.split(":")[1],
                                     res.value.values[:])

    def __rpc_with_single_op(self, optype, partition, content, id=None,
                             retry=0):
        idcard = None
        try:
            idcard = str(id)
        except Exception:
            raise PfGrpcParamTypeError("param id must can be stringfy")

        if not isinstance(content, str) and not isinstance(content, int):
            raise PfGrpcParamTypeError("content must be string or int")

        ic = content
        if idcard:
            ic = "%s:%s" % (idcard, content)
        op = PhxPFSingleOperator()
        op.operator = optype
        op.args.partition = str(partition)
        op.args.value = ic

        if optype == PHXPF_SINGLE_FILTER:
            res = self.__stub.FilterOne(op)
        if optype == PHXPF_SINGLE_UPDATE:
            res = self.__stub.UpdateOne(op)
        if optype == PHXPF_SINGLE_DELETE:
            res = self.__stub.DeleteOne(op)

        if res.ret == PHXPARTITIONFILTER_NOTMASTER and retry > 0:
            return self.__rpc_with_single_op(
                optype, partition, content, retry - 1)

        if res.ret == PHXPARTITIONFILTER_CLIENT_RETRY and retry > 0:
            return self.__rpc_with_single_op(
                optype, partition, content, retry - 1)

        if res.ret != PHXPARTITIONFILTER_OK:
            raise PfGrpcReturnError(res.msg)

        return res.value

    def filter(self, partition, contents, id=None):
        return self.__rpc_with_op(
            PHXPF_FILTER, partition, contents, id, self.retry)

    def update(self, partition, contents, id=None):
        return self.__rpc_with_op(
            PHXPF_UPDATE, partition, contents, id, self.retry)

    def delete(self, partition, contents, id=None):
        return self.__rpc_with_op(
            PHXPF_DELETE, partition, contents, id, self.retry)

    def filterone(self, partition, content, id=None):
        return self.__rpc_with_single_op(
            PHXPF_SINGLE_FILTER, partition, content, id, self.retry)

    def updateone(self, partition, content, id=None):
        return self.__rpc_with_single_op(
            PHXPF_SINGLE_UPDATE, partition, content, id, self.retry)

    def deleteone(self, partition, content, id=None):
        return self.__rpc_with_single_op(
            PHXPF_SINGLE_DELETE, partition, content, id, self.retry)


if __name__ == "__main__":
    import random

    client = PhxPfGrpcClient([
        #    ("b-ad-core03", "11263"),
        ("b-ad-core03", "11264"),
        #    ("b-ad-core03", "11265")
    ])
    seed = random.randint(100000000000000000, 999999999999999999)
    data = range(seed, seed + 100)
    res = client.filter("20180904", data, id=18555476)
    print res
    client.update("20180904", data, id=18555476)
    res = client.filter("20180904", data, id=18555476)
    print res
    client.delete("20180904", data, id=18555476)
    res = client.filter("20180904", data, id=18555476)
    print res
