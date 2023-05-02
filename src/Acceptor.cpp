#include"Acceptor.h"
#include"socket.h"
#include"InetAddress.h"
#include"Channel.h"
#include"server.h"

Acceptor::Acceptor(EventLoop *_loop) : loop(_loop)
{
    Sock = new sock();
    addr = new InetAddress("127.0.0.1",8888);
    Sock->bind(addr);
    Sock->listen();
    Sock->setnonblocking();
    acceptChannel = new Channel(loop,Sock->getFd());
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
    acceptChannel->setCallback(cb);
    acceptChannel->enableReading();
}

Acceptor::~Acceptor(){
    delete Sock;
    delete addr;
    delete acceptChannel;
}

void Acceptor::acceptConnection(){
    newConnectionCallback(Sock);
}

void Acceptor::setNewConnectionCallback(std::function<void(sock*)> _cb){
    newConnectionCallback = _cb;
}
