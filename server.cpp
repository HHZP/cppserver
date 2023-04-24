#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include"util.h"//包含errif函数，用于异常监测
#include<unistd.h>
#include<sys/epoll.h>
#include<errno.h>
#include<fcntl.h>
#include"InetAddress.h"
#include"Epoll.h"
#include<iostream>
#include"socket.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd){//设置为非阻塞
    fcntl(fd,F_SETFL,fcntl(fd,F_GETFL) | O_NONBLOCK);
}

void handleReadEvent(int);

int main(){
    sock *ser_sock = new sock();
    InetAddress *ser_addr = new InetAddress("127.0.0.1",8888);
    ser_sock->bind(ser_addr);
    ser_sock->listen();
    
    Epoll *ep = new Epoll();
    ser_sock->setnonblocking();

    ep->addFd(ser_sock->getFd(),EPOLLIN | EPOLLET);

    while(true){
        std::vector<epoll_event> events = ep->poll();
        int nfds = events.size();
        for(int i=0;i<nfds;i++){
            if(events[i].data.fd == ser_sock->getFd()){
                InetAddress *clnt_addr = new InetAddress();
                sock *clnt_sock = new sock(ser_sock->accept(clnt_addr));
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
                clnt_sock->setnonblocking();
                ep->addFd(clnt_sock->getFd(),EPOLLIN | EPOLLET);
            }
            else if(events[i].events & EPOLLIN){
                handleReadEvent(events[i].data.fd);
            }
            else {
                printf("something else happened\n");
            }
        }
    }
    delete ser_sock;
    delete ser_addr;
}

void handleReadEvent(int sockfd){
    char buf[READ_BUFFER];
    while(true){
        bzero(&buf,sizeof(buf));
        ssize_t bytes_read = read(sockfd,buf,sizeof(buf));

        if(bytes_read > 0){
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf,sizeof(buf));
        }
        else if(bytes_read == -1 && errno == EINTR){
            printf("continue reading");
            continue;
        }
        else if(bytes_read == -1 &&((errno == EAGAIN) ||(errno == EWOULDBLOCK))){
            printf("finish reading once, errno: %d\n", errno);
            break;
        }
        else if(bytes_read == 0){
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);
            break;
        }
    }
}

