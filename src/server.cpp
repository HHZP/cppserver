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
#include<iostream>
#include"socket.h"
#include"Channel.h"
#include"server.h"
#include"Acceptor.h"
#include<functional>

#define READ_BUFFER 1024

Server::Server(EventLoop *_loop) : loop(_loop),acceptor(nullptr){
    acceptor = new Acceptor(loop);
    std::function<void(sock*)> cb = std::bind(&Server::newConnection,this,std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);
}

Server::~Server(){
    delete acceptor;
}

void Server::handleReadEvent(int sockfd){
    char buf[READ_BUFFER];
    while(true){
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd,buf,sizeof(buf));
        if(bytes_read > 0){
            printf("message from client fd %d: %s\n",sockfd,buf);
            write(sockfd,buf,sizeof(buf));
        }else if(bytes_read == -1 && errno == EINTR){ //客户端正常中断，继续读取
            printf("continue reading");
            continue;
        }else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//读取完毕
            printf("finsh reading once, errno: %d\n",errno);
            break;
        }
        else if(bytes_read == 0){
            printf("EOF, client fd % disconnectioned\n",sockfd);
            close(sockfd);
            break;
        }
    }
}

void Server::newConnection(sock *ser_sock){
    InetAddress *clnt_addr = new InetAddress();
    sock *clnt_sock = new sock(ser_sock->accept(clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n",clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr),ntohs(clnt_addr->addr.sin_port));
    clnt_sock->setnonblocking();
    Channel *clntChannel = new Channel(loop, clnt_sock->getFd());
    std::function<void()> cb =std::bind(&Server::handleReadEvent, this, clnt_sock->getFd());
    clntChannel->setCallback(cb);
    clntChannel->enableReading();
}
