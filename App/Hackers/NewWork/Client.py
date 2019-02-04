# -*- coding: utf-8 -*-
__author__ = 'Saber'
__date__ = '2019/1/28 0028'

import socket
import threading

target_host = 'www.baidu.com'
target_port = 80


def Tcp_Client():
    """
    TCP客户端
    :return:
    """
    target_host = "192.168.1.108"
    target_port = 9999

    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)              # AF_INET --> 使用标准的IPv4地址或者主机名
    client.connect((target_host, target_port))
    # client.send(b"GET / HTTP/1.1\r\nHost:www.baidu.com\r\n\r\n")
    client.send(b'Hello World!')
    response = client.recv(4096)
    print(response)


def Udp_Client():
    """
    UDP客户端
    :return:
    """
    target_host = "127.0.0.1"
    target_host = 80

    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    client.sendto("AABBCC", (target_host, target_port))
    data, addr = client.recvfrom(4096)
    print("data:", data)
    print("addr:", addr)


if __name__ == '__main__':
    Tcp_Client()

