/*
* multi_thread_socket_server.c
*
*  Created on: Mar 14, 2014
*      Author: nerohwang
*/
#include<stdlib.h>
#include<pthread.h>
#include<sys/socket.h>
#include<sys/types.h>       //pthread_t , pthread_attr_t and so on.
#include<stdio.h>
#include<netinet/in.h>      //structure sockaddr_in
#include<arpa/inet.h>       //Func : htonl; htons; ntohl; ntohs
#include<assert.h>          //Func :assert
#include<string.h>          //Func :memset
#include<unistd.h>          //Func :close,write,read
#include<fcntl.h>
#define BUFFER_LENGTH 200
#define MAX_CONN_LIMIT 200     //MAX connection limit
int  g_sockport=0;
int  g_isEnc=0;
int  g_encKey=0x00;
pthread_mutex_t mutex;
static void Data_handle(void * sock_fd);   //Only can be seen in the file
int main(int argc,char *argv[])
{   
    int sockfd_server;
    int sockfd;
    int fd_temp;
    struct sockaddr_in s_addr_in;
    struct sockaddr_in s_addr_client;
    int client_length;
if(argc < 2)
{
  printf("Use:./elf port [isEnc 0 or 1] [encKey]\n");
  return 0;
}
if(argc >= 2){
g_sockport = (short)(atoi(argv[1]));
g_isEnc = atoi(argv[2]);
g_encKey = (char)(atoi(argv[3]));
}
    pthread_mutex_init(&mutex,NULL);
    sockfd_server = socket(AF_INET,SOCK_STREAM,0);  //ipv4,TCP
    assert(sockfd_server != -1);
    //before bind(), set the attr of structure sockaddr.
    memset(&s_addr_in,0,sizeof(s_addr_in));
    s_addr_in.sin_family = AF_INET;
    s_addr_in.sin_addr.s_addr = htonl(INADDR_ANY);  
    s_addr_in.sin_port = htons(g_sockport);          
    fd_temp = bind(sockfd_server,(struct scokaddr *)(&s_addr_in),sizeof(s_addr_in));
    if(fd_temp == -1)
    {
        fprintf(stderr,"bind error!\n");
        exit(1);
    }
    fd_temp = listen(sockfd_server,MAX_CONN_LIMIT);
printf("[+]Listening on port %d\n",g_sockport);
    if(fd_temp == -1)
    {
        fprintf(stderr,"listen error!\n");
        exit(1);
    }
    while(1)
    { 
        pthread_t thread_id;
        client_length = sizeof(s_addr_client);
        //Block here. Until server accpets a new connection.
        sockfd = accept(sockfd_server,(struct sockaddr_*)(&s_addr_client),(socklen_t *)(&client_length));
        if(sockfd == -1)
        {
            fprintf(stderr,"Accept error!\n");
            continue;                              
        }
printf("[+]**********************************...\n");
printf("[+]Got a connection from %s : %d\n",inet_ntoa(s_addr_client.sin_addr),ntohs(s_addr_client.sin_port) );
        if(pthread_create(&thread_id,NULL,(void *)(&Data_handle),(void *)(&sockfd)) == -1)
        {
            fprintf(stderr,"pthread_create error!\n");
            break;                                 
        }
    }
    //Clear
    int ret = shutdown(sockfd_server,SHUT_WR); //shut down the all or part of a full-duplex connection.
    assert(ret != -1);
    printf("Server shuts down\n");
    pthread_mutex_destroy(&mutex);
    return 0;
}
void Encrypt(char * pSrc, char * pEnc, int len)
{
   int i;
   for(i = 0; i < len; i++)
    {
pEnc[i] = pSrc[i] ^ g_encKey;
    }
}
void Decrypt(char * pEnc, char * pDec, int len)
{
   int i;
   for(i = 0; i < len; i++)
    {
pDec[i] = pEnc[i] ^ g_encKey;
    }
}
void writeToFile(char *pFlag, int len){
    int fd;
int bytes=0;
    char *path="./flaglist";
pthread_mutex_lock(&mutex);
    fd=open(path,O_WRONLY|O_APPEND);
if(fd==-1)
    {
        printf("[+]ErrorOpeningFile\n");
pthread_mutex_unlock(&mutex);
return;
    }
pFlag[len]='\n';
    bytes=write(fd,pFlag,len+1);
    if(bytes==-1 || bytes==0)
    {
        printf("[+]ReadFailed.\n");
pthread_mutex_unlock(&mutex);
return;
    }
    close(fd);
pthread_mutex_unlock(&mutex);
}
static void Data_handle(void * sock_fd)
{
    int fd = *((int *)sock_fd);
    int i_recvBytes;
    char data_recv[BUFFER_LENGTH];
    char data_dec[BUFFER_LENGTH];
    memset(data_recv,0,BUFFER_LENGTH);
    memset(data_dec,0,BUFFER_LENGTH);
    i_recvBytes = read(fd,data_recv,BUFFER_LENGTH);

    if(i_recvBytes == 0 || i_recvBytes == -1)
    {
      printf("Maybe the client has closed or read error!\n");     
    }
    if(g_isEnc){
  Decrypt(data_recv,data_dec,strlen(data_recv));
  printf("[+]Read from client Enc : %s,Lenth:%d \n",data_dec,strlen(data_dec));
  writeToFile(data_dec,strlen(data_dec));
}else
{
      printf("[+]Read from client : %s,Lenth:%d \n",data_recv,strlen(data_recv));
  writeToFile(data_recv,strlen(data_recv));
}
    printf("[+]Terminating current client_connection...\n\n");
    close(fd);            
    pthread_exit(NULL);   
}
