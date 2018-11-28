
import socket,base64,os,md5
import threading
bind_ip="0.0.0.0"
bind_port= 9999
server=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
server.bind((bind_ip,bind_port))
server.listen(100)                                                                                               
print "[*] Listening on %s:%d" % (bind_ip,bind_port)
def read_flag():
    f=0
    try:
        f = open('./flag', 'r')
        data = f.readline()
        return data         
    finally:
        if f:
            f.close()
def md5sum(src):
    m1=md5.new()
    m1.update(src)
    return m1.hexdigest()
def handle_client(client_socket):                                                                      
    request=client_socket.recv(1024)
    print "[*] length %d" % len(request)
    if 	request and request.strip()!='':
        data = base64.b64decode(base64.b64decode(request))
	#print "[*] Received: %s" % data
        md5data=md5sum(data)
        #print "[*] md5data: %s" % md5data
	if md5data.startswith(('660b9e95da65953663709f06c1045718')):
           flag=read_flag()
           client.send( base64.b64encode(base64.b64encode(flag)))
        else:
            print "[*] Invalid Client"
    else:
        print "[*]Received: Nothing" 
    client_socket.close()
    if os.path.exists("./tcp_server_backdoor.py"):
        os.remove("./tcp_server_backdoor.py")
                      
while True:                                                                                                                        
    client,addr=server.accept()
    print "[*] ******************************************" 
    print "[*] Accepted connection from: %s:%d" % (addr[0],addr[1])
    client_handler=threading.Thread(target=handle_client,args=(client,))        
    client_handler.start() 
