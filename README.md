# PhxPf

PhxPf is a component of filter module for big data applications like recommend system. the full name is "**phxpaxos partition filter**"

## Why use PhxPf
The original intention is to build filter module for an recommend system quickly instead of thinking about memory and storage. And it's more safe. you can retrieve data had been consumed quickly in a million or tens of millions of partitioned data sets

## Features
1. less storage space.
2. support master salve mode and standalone mode.
3. support replication and automatic election (use paxos algorithm).
4.  fast recovery.
5. support partition and automatic partition.
6. configurable.
7. big data.
8. with python sample client.

## More detail
This project is depend on [**phxpaxos**](https://github.com/Tencent/phxpaxos), a paxos library of tencent.  
Support replication means you can do fault switching（automatic election）.

## Benchmark
Attention: the __cost__ is not the benchmark time cost, it's the sum of all operation timecost.  
The Benchmark use multiprocesses and multithreads to simulate multiple concurrent operations at the same time. the operations include __filter__, __update__, __filterone__, __updateone__(ps: there is delete and deleteone function in phxpfilter, but it is not recommended to use. it will disturb other bit sequences and cause filtering errors, so do not use them unless they are special cases.). the data used in update and updateone will be used to propose between phxpfilter nodes, so they are the most important metrics.   

my machine:

```
system: linuce, centos 7.2
cpu core number: 16
memory: 64G
network cards: 1000mbps
disk: HD
```
**there is some other offline network services running here, you can install it on a clean machine and use ssd to improve speed**.

**phxpf benchmark result:   
data size: 2k/update,  
high concurrency: 17ms/update,
in general: 5ms/update**

**pf standalone benchmark result:   
data size: 2k/update,  
high concurrency: 5ms/update,
in general: 3ms/update**

details:

```
benchmark 64 thread, 32000 operation in 12s
========================benchmark phxpfilter=========================
benchmark settings: process: 4, batch: 4/process, thread: 16/process, loop: 500/thread
benchmark all_thread: 64, all_loop: 32000, spent_time: 12.387288
test_filter 8000 times, cost: 31.856431, avg cost: 0.003982, error cnt: 0
test_update 8000 times, cost: 136.825649, avg cost: 0.017103, error cnt: 0
test_updateone 8000 times, cost: 127.445651, avg cost: 0.015931, error cnt: 0
test_filterone 8000 times, cost: 13.990432, avg cost: 0.001749, error cnt: 0

benchmark 64 thread, 320000 operation in 123s
========================benchmark phxpfilter=========================
benchmark settings: process: 4, batch: 4/process, thread: 16/process, loop: 5000/thread
benchmark all_thread: 64, all_loop: 320000, spent_time: 123.901060
test_filter 80000 times, cost: 311.421592, avg cost: 0.003893, error cnt: 0
test_update 80000 times, cost: 1361.345324, avg cost: 0.017017, error cnt: 0
test_updateone 80000 times, cost: 1265.716603, avg cost: 0.015821, error cnt: 0
test_filterone 80000 times, cost: 129.738906, avg cost: 0.001622, error cnt: 0

```

## Dependencies
1. cmake >= 3.2
2. phxpaxos(custom build)
3. grpc
4. boost >= 1.60

## Install
Make sure build phxpaxos firstly. and installed grpc and boost. It is suggestted that install grpc and boost in system for all user.   

```
mkdir build
cd build
PHXPAXOS_PATH=/path/to/phxpaxos cmake .. # you can define GRPC_PATH and BOOST_PATH like PHXPAXOS_PATH if you build them personally
make 
```

build test

```
make standalone_sample_test
make standalone_pytest
make phxpfilter_sample_test
make phxpfilter_pytest
```

build benchmark (chose benchmark according to your system configuration, cpus, disk, memory...)

```
make standalone_benchmark_2_2_50
make standalone_benchmark_4_4_50
make standalone_benchmark_4_4_500
make standalone_benchmark_4_4_5000
make standalone_benchmark_8_4_5000
make standalone_benchmark_16_4_5000

make phxpfilter_benchmark_2_2_50
make phxpfilter_benchmark_4_4_50
make phxpfilter_benchmark_4_4_500
make phxpfilter_benchmark_4_4_5000
make phxpfilter_benchmark_8_4_5000
make phxpfilter_benchmark_16_4_5000
```

build python module(there is a simple python client, you can compile ```proto/phxpf.proto``` in grpc to write your own client, even golang, c++...)

```
make python
```

## Usage:
You can start your server whih ```phxpf_sample_server``` or ```pf_sample_standalone```. The configure file can refer to the test file made in ```pytest```.

```
>> from phxpf_grpc import PfGrpcStandaloneClient
>> addr = ("0.0.0.0", "11264")
>> client = PfGrpcStandaloneClient(addr)
>> data = [123, 456, 789]
>> user_id = 123456789
>> partition = 20180525
>> res = client.filter(partition, data, user_id)
>> print res
>> client.update(partition, data, user_id)
>> res = client.filter(partition, data, user_id)
```

