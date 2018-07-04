#!/usr/bin/env python
# coding: utf-8

import sys
import random
import time

from multiprocessing import Queue
from collections import defaultdict
from pftesttool import PFBenchmarkProcess
from phxpf_grpc import PfGrpcStandaloneClient

if __name__ == "__main__":
    process_num = int(sys.argv[1])
    batch_num = int(sys.argv[2])
    loop = int(sys.argv[3])

    addr = ("127.0.0.1", "11264")

    ts = []
    baseid = random.randint(100000, 999999)
    report_queue = Queue(1024)
    cost_counter = defaultdict(lambda: {"cost": 0.0, "cnt": 0, "error": 0})

    for i in range(process_num):
        ts.append(
            PFBenchmarkProcess(
                baseid + i*1000, report_queue,
                addr, PfGrpcStandaloneClient, batch_num=batch_num, loop=loop))

    t_start = time.time()
    for t in ts:
        t.daemon=True
        t.start()

    discount = process_num * batch_num * len(PFBenchmarkProcess.test_funcs)

    while discount:
        r = report_queue.get()
        if r[0] == "done":
            if not r[2]:
                print r[3]
            discount -= 1
            continue

        cost_counter[r[0]]["cost"] += r[1]
        cost_counter[r[0]]["cnt"] += 1
        if not r[2]:
            cost_counter[r[0]]["error"] += 1
    t_end = time.time()

    all_thread = process_num * batch_num * len(PFBenchmarkProcess.test_funcs)
    print "========================benchmark standalone========================="
    print "benchmark settings: process: %d, batch: %d/process, thread: %d/process, loop: %d/thread" % \
          (process_num, batch_num, batch_num * len(PFBenchmarkProcess.test_funcs), loop)
    print "benchmark all_thread: %d, all_loop: %d, spent_time: %.6f" % (
        all_thread, all_thread*loop, t_end - t_start)
    for func, cost in cost_counter.items():
        print "%s %d times, cost: %.6fs, avg cost: %.6fs, error cnt: %d" % \
              (func, cost["cnt"], cost["cost"],
               cost["cost"] / cost["cnt"], cost["error"])
