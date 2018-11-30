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
int i =0;
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
    while(1)
    {   
    
        pthread_t thread_id;
        char *ips[] = {"172.5.70.153","172.5.70.153"};
    for(i=0; i<1;i++){
  
          if(pthread_create(&thread_id,NULL,(void *)(&Data_handle),(void *)(ips[i])) == -1)
          {
            fprintf(stderr,"pthread_create error!\n");
            break;                                 
          }
}
sleep(5);
    }
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
static void Data_handle(void * p_ip)
{   
    int bytes=0;
int result=-1;
int sockfd;
struct sockaddr_in s_addr_in;
    char data_recv[BUFFER_LENGTH];
    char data_dec[BUFFER_LENGTH];
    memset(data_recv,0,BUFFER_LENGTH);
    memset(data_dec,0,BUFFER_LENGTH);
    //before func connect, set the attr of structure sockaddr.
    sockfd = socket(AF_INET,SOCK_STREAM,0);       //ipv4,TCP
    memset(&s_addr_in,0,sizeof(s_addr_in));
    s_addr_in.sin_addr.s_addr = inet_addr((char*)p_ip);      //trans char * to in_addr_t
    s_addr_in.sin_family = AF_INET;
    s_addr_in.sin_port = htons(g_sockport);
    result = connect(sockfd,(struct sockaddr *)(&s_addr_in),sizeof(s_addr_in));
    if(result == -1)
    {
        fprintf(stderr,"Connect error! \n");
        shutdown(sockfd,SHUT_WR);
pthread_exit(NULL); 
return;
    }
//write(sockfd,"Hello",strlen("Hello"));
bytes = read(sockfd,data_recv,BUFFER_LENGTH);

    if(bytes == 0 || bytes == -1)
    {
      printf("Maybe the Server has closed or read error!\n"); 
      shutdown(sockfd,SHUT_WR);
  pthread_exit(NULL); 
  return;  
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
    shutdown(sockfd,SHUT_WR);       //or you can use func close()           
    pthread_exit(NULL);   
}
