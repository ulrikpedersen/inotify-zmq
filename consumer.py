#!/bin/env dls-python

from pkg_resources import require
require('pyzmq')

import zmq

def consumer():
    context = zmq.Context()
    # recieve work
    consumer_receiver = context.socket(zmq.PULL)
    consumer_receiver.connect("tcp://127.0.0.1:9988")

    while True:
        msg = consumer_receiver.recv_json()
        print msg

consumer()

