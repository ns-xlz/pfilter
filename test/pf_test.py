#!/usr/bin/env python
# coding: utf-8

import unittest

from pftesttool import PfTestClient

from phxpf_grpc import PfGrpcStandaloneClient

rpc_addrs = ("0.0.0.0", 11264)


class TestPf(unittest.TestCase):
    def setUp(self):
        self.client = PfTestClient(PfGrpcStandaloneClient(rpc_addrs))

    def test_filter(self):
        self.assertTrue(self.client.test_filter())

    def test_update(self):
        self.assertTrue(self.client.test_update())

    def test_delete(self):
        self.assertTrue(self.client.test_delete())

    def test_filterone(self):
        self.assertTrue(self.client.test_filterone())

    def test_updateone(self):
        self.assertTrue(self.client.test_updateone())

    def test_deleteone(self):
        self.assertTrue(self.client.test_deleteone())

    def test_operator_chain(self):
        self.assertTrue(self.client.test_operator_chain())


if __name__ == "__main__":
    unittest.main()
