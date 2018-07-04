#!/usr/bin/env python
# coding: utf-8

import random
import time
import traceback

from threading import Thread
from multiprocessing import Process
from Queue import Queue
from itertools import cycle


def rand_data(num):
    data = random.randint(100000000000000000, 999999999999999999)
    return range(data, data + num)


def rand():
    return random.randint(100000000000000000, 999999999999999999)


class PfTestClient(object):
    def __init__(self, client, fid=None, partition=None):
        self.client = client
        self.partition = partition or time.strftime("%Y%m%d")
        self.fid = fid or 1001
        self.num = 100

    def test_filter(self):
        try:
            data = rand_data(self.num)
            res = self.client.filter(self.partition, data, self.fid)
            return True
        except Exception:
            return False

    def test_update(self):
        try:
            data = rand_data(self.num)
            res = self.client.update(self.partition, data, self.fid)
            return True
        except Exception:
            return False

    def test_delete(self):
        try:
            data = rand_data(self.num)
            res = self.client.delete(self.partition, data, self.fid)
            return True
        except Exception:
            return False

    def test_filterone(self, *args, **kwargs):
        try:
            data = rand()
            res = self.client.filterone(self.partition, data, self.fid)
            return True
        except Exception:
            return False

    def test_updateone(self, *args, **kwargs):
        try:
            data = rand()
            res = self.client.updateone(self.partition, data, self.fid)
            return True
        except Exception:
            return False

    def test_deleteone(self, *args, **kwargs):
        try:
            data = rand()
            self.client.deleteone(self.partition, data, self.fid)
            return True
        except Exception:
            return False

    def test_operator_chain(self):
        try:
            data = rand_data(self.num)
            res = self.client.filter(self.partition, data, self.fid)
            if len(set(res) - set(data)) > 0:
                return False
            self.client.update(self.partition, data, self.fid)
            res = self.client.filter(self.partition, data, self.fid)
            if res:
                print res
                return False
            # self.client.delete(self.partition, data, self.fid)
            # res = self.client.filter(self.partition, data, self.fid)
            # if len(set(res) - set(data)) > 0:
            #     return False
            return True
        except Exception:
            return False


class PFBenchmarkTread(Thread):
    def __init__(self, report_queue, client, test_func, **kwargs):
        super(PFBenchmarkTread, self).__init__()
        self.report_queue = report_queue
        self.id = kwargs.get("id")
        self.partition = kwargs.get("partition", time.strftime("%Y%m%d"))
        self.loop = kwargs.get("loop", 1000)
        self.client = PfTestClient(client, self.id, self.partition)
        self.test_func_name = test_func
        self.test_func = getattr(self.client, test_func)

    def report(self, func, timecost, succ, msg=None):
        self.report_queue.put((func, timecost, succ, msg))

    def run(self):
        i = 0
        while i < self.loop:
            try:
                t_filter_b = time.time()
                res = self.test_func()
                t_filter_e = time.time()
                self.report(self.test_func_name, t_filter_e - t_filter_b, res)
            except Exception as e:
                self.report("done", 0, False, traceback.format_exc().replace("\n", "\t"))
                break
            i += 1
        self.report("done", 0, True)


class PFBenchmarkProcess(Process):
    test_funcs = [
        "test_filter", "test_update", "test_filterone", "test_updateone",
        "test_operator_chain"
    ]

    def __init__(self, baseid, report_queue, rpc_addr, client_cls, **kwargs):
        super(PFBenchmarkProcess, self).__init__()
        self.baseid = int(baseid)
        self.report_queue = report_queue
        self.rpc_addr = rpc_addr
        self.client_cls = client_cls
        self.batch_num = kwargs.get("batch_num", 4)
        self.loop = kwargs.get("loop", 1000)

    def run(self):
        threads = []
        j = 0
        partitions = cycle(["20171212", "20171213", "20171214"])
        for i in range(self.batch_num):
            for func in self.test_funcs:
                j += 1
                client = self.client_cls(self.rpc_addr)
                t = PFBenchmarkTread(
                    self.report_queue, client, func,
                    id=self.baseid+j, loop=self.loop, partition=partitions.next())
                t.start()
                threads.append(t)

        for t in threads:
            t.join()
