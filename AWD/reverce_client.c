/*
* socket_client.c
*
*  Created on: Mar 15, 2014
*/
#include<stdlib.h>
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
void read_file(char * p_src){
    int fd;
int bytes=0;
    char *path="./flag";
    fd=open(path,O_RDONLY);
if(fd==-1)
    {
        printf("ErrorOpeningFile\n");
        exit(1);
    }
    bytes=read(fd,p_src,100);
    if(bytes==-1 || bytes==0)
    {
        printf("ReadFailed.\n");
close(fd);
        exit(1);
    }
    close(fd);
}
int main(int argc,char *argv[])
{
    int sockfd;
    int result;
int bytes=0; 
    struct sockaddr_in s_addr_in;
    char data_src[BUFFER_LENGTH];
    char data_enc[BUFFER_LENGTH];
    if(sockfd == -1)
    {
        fprintf(stderr,"socket error!\n");
        exit(1);
    }
    if(argc < 3)
{
  printf("Use:./elf ip port [isEnc 0 or 1] [encKey]\n");
  return 0;
}
if(argc >=3){
  g_sockport = (short)(atoi(argv[2]));
  g_isEnc = atoi(argv[3]);
  g_encKey = (char)(atoi(argv[4]));
}
   while(1)
    {
memset(data_src,0,BUFFER_LENGTH);
    memset(data_enc,0,BUFFER_LENGTH);
    //before func connect, set the attr of structure sockaddr.
    sockfd = socket(AF_INET,SOCK_STREAM,0);       //ipv4,TCP
    memset(&s_addr_in,0,sizeof(s_addr_in));
    s_addr_in.sin_addr.s_addr = inet_addr(argv[1]);      //trans char * to in_addr_t
    s_addr_in.sin_family = AF_INET;
    s_addr_in.sin_port = htons(g_sockport);
    result = connect(sockfd,(struct sockaddr *)(&s_addr_in),sizeof(s_addr_in));
    if(result == -1)
    {
        fprintf(stderr,"Connect error! \n");
        shutdown(sockfd,SHUT_WR);
sleep(5);
continue;
    }
read_file(data_src);
if(g_isEnc){
  Decrypt(data_src,data_enc,strlen(data_src));
  bytes = write(sockfd,data_enc,strlen(data_enc));
}else
{
      bytes = write(sockfd,data_src,strlen(data_src));
}
    if(bytes == -1)
    {
      fprintf(stderr,"write error\n");
    }
    int ret = shutdown(sockfd,SHUT_WR);       //or you can use func close()--<unistd.h> to close 
    assert(ret != -1);
sleep(5); 
}
    return 0;
}
