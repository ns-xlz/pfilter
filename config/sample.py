#!/usr/bin/env python
# coding: utf-8

import os
import sys
import json
import shutil

max_partition = 3


def check_remove_create(loc):
    if os.path.exists(loc):
        shutil.rmtree(loc)

    os.mkdir(loc)

    return loc


def check_create_dir(location_dir, sub_dir):
    loc = os.path.join(location_dir, sub_dir)
    return check_remove_create(loc)


def mk_sample_log(location):
    return check_create_dir(location, "log")


def mk_sample_instance(location):
    return check_create_dir(location, "phx")


def mk_sample_data(location):
    return check_create_dir(location, "data")


def mk_sample(sample_location):
    return check_create_dir(sample_location, "sample")


def mk_config(location):
    return check_create_dir(location, "config")


def mk_pf_config(config_loc, instance_loc, bloom_loc, index):
    config_file = os.path.join(config_loc, "pfconfig%d.json" % int(index))
    config = {
        "instance_path": check_create_dir(instance_loc, "phx%d" % int(index)),
        "bloom_path": check_create_dir(bloom_loc, "bloom%d" % int(index)),
        "bloom_max_items": 1000000,
        "bloom_error_rate": 0.00001,
        "bloom_hash_seed": 0,
        "bloom_max_partitions": max_partition,
        "bloom_hash_func": "murmurhash128",
    }

    with open(config_file, "w") as conf:
        json.dump(config, conf)

    return config_file


def mk_phxpf_config(config_loc, phxlog_loc, syslog_loc, pfconf_file, index):
    config_file = os.path.join(config_loc, "phxpf%d.json" % int(index))
    config = {
        "io_thread_count": 1,
        "batch_size": 1000,
        "batch_timeout_ms": 10,
        "use_master": 1,
        "log_level": 3,
        "max_log_size": 128,  # m
        "rpc_addr": "127.0.0.1:%d" % (11261 + int(index) - 1),
        "nodeaddr": "127.0.0.1:%d" % (11111 + int(index) - 1),
        "nodelist": "127.0.0.1:11111,127.0.0.1:11112,127.0.0.1:11113",
        "paxos_log_path": check_create_dir(phxlog_loc, "phx%d" % int(index)),
        "sys_log_path": check_create_dir(syslog_loc, "log%d" % int(index)),
        "pf_config_file": pfconf_file,
    }

    with open(config_file, "w") as conf:
        json.dump(config, conf)

    return config_file


def usage(argv):
    print "Usage: %s <sample_dir> <nodenum>" % argv
    exit(-1)


def create_config_with_dir(work_dir, nodenum):
    sample_work_dir = mk_sample(work_dir)
    sample_config_dir = mk_config(sample_work_dir)
    sample_log_dir = mk_sample_log(sample_work_dir)
    sample_instance_dir = mk_sample_instance(sample_work_dir)
    sample_data_dir = mk_sample_data(sample_work_dir)
    for i in range(1, int(nodenum) + 1):
        pf_config = mk_pf_config(
            sample_config_dir, sample_instance_dir, sample_data_dir, i)
        phxpf_cofnig = mk_phxpf_config(
            sample_config_dir, sample_log_dir, sample_log_dir, pf_config, i)
        print "partition filter config: %s done" % pf_config
        print "phxpaxos partition filter config: %s done" % phxpf_cofnig


if __name__ == "__main__":
    print sys.argv
    if len(sys.argv) < 3:
        usage(sys.argv[0])

    work_dir = sys.argv[1]
    nodenum = int(sys.argv[2])

    if len(sys.argv) > 3:
        max_partition = int(sys.argv[3])

    check_remove_create(work_dir)
    create_config_with_dir(work_dir, nodenum)
