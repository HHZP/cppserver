#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include"src/util.h"

#define BUFER_SIZE 1024

int main() {
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    errif(sockfd == -1,"socket create error");
    
    struct sockaddr_in ser_addr;
    bzero(&ser_addr,sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ser_addr.sin_port=htos(8888);

    errif(connect(sockfd, (sockaddr *)&ser_addr,sizeof(ser_addr)) == -1,"socket connect error");

    while(true){
        char buf[BUFFER_SIZE];
        bzero(&buf,sizeof(buf));
        scanf("%s",buf);
        ssize_t write_bytes = write(sockfd ,buf ,sizeof(buf));
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        bzero(&buf,sizeof(buf));
        ssize_t read_bytes =read(sockfd, buf, sizeof(buf));
        if(read_bytes > 0){
            printf("message from server: %s\n",buf);
        }else if(read_bytes == 0){
            printf("server socket disconnected!\n");
            break;
        }else if(read_bytes == -1){
            close(sockfd);
            errif(true,"socket read erro");
        }
    }
    close(sockfd);
    return 0;
}
