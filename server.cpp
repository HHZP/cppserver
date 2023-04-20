#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include"util.h"//包含errif函数，用于异常监测
#include<unistd.h>

int main(){
    int sockfd= socket(AF_INET, SOCK_STREAM,0); //创建socket IP地址类型，所用传输协议
    errif(sockfd == -1,"socket create error");


    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serv_addr.sin_port=htons(8888);

    errif(bind(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr)) == -1,"socket bind error");

    errif(listen(sockfd,SOMAXCONN)== -1, "socket listen error");
    
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    bzero(&clnt_addr,sizeof(clnt_addr));

    int clnt_sockfd = accept(sockfd,(sockaddr*)&clnt_addr,&clnt_addr_len);

    errif(clnt_sockfd==-1,"socket accept error");

    printf("new client fd %d! IP: %s Port: %d\n",clnt_sockfd,inet_ntoa(clnt_addr.sin_addr),ntohs(clnt_addr.sin_port));
    
    while(true){
        char buf[1024];//缓冲区
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes= read(clnt_sockfd,buf, sizeof(buf));
        if(read_bytes > 0){
            printf("message from client fd %d: %s\n",clnt_sockfd,buf);
            write(clnt_sockfd,buf,sizeof(buf));
        }
        else if(read_bytes==0){
            printf("client fd %d disconnected\n",clnt_sockfd);
            close(clnt_sockfd);
            break;
        }
        else if(read_bytes == -1){
            close(clnt_sockfd);
            errif(true,"socket read error");
        }
    }
    close(sockfd);//最后记住要确保fd关闭，因为理论上fd是有限的
    return 0;
    
}
