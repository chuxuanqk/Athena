# -*- coding: utf-8 -*-
__author__ = 'Saber'
__date__ = '2019/2/3 0003'


import socket
import threading

def Tcp_Server():
    """
    TCP服务器
    :return:
    """
    bind_ip = "0.0.0.0"
    bind_port = 9999

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    server.bind((bind_ip, bind_port))
    server.listen(5)

    print("[*] Listening on %s:%d" %(bind_ip, bind_port))

    while True:
        client, addr = server.accept()
        print("[*] Accepted connect from:%s:%d" % (addr[0], addr[1]))

        # 挂起客户端线程， 处理传入的数据
        client_handler = threading.Thread(target=handle_client, args=(client,))
        client_handler.start()


def handle_client(client_socket):
    """
    客户处理线程
    :param client_socket:
    :return:
    """
    request = client_socket.recv(1024)
    print("[*] Received:%s" %request)

    # 返还一个数据包
    client_socket.send(b"ACK!")

    client_socket.close()


if __name__ == '__main__':
    Tcp_Server()
