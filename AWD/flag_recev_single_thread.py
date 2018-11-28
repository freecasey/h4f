#!/usr/bin/env python
#coding:utf-8
import socket, base64
  
def handle_request(client):
    buf = client.recv(1024)
    print('Recv :' + str(client.getpeername()) +'  flag:' + base64.b64decode(base64.b64decode(buf)))
  
def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(('0.0.0.0',8080))
    sock.listen(24)
  
    while True:
        connection, address = sock.accept()vi 
        handle_request(connection)
        connection.close()
if __name__ == '__main__':
  main()
