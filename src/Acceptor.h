#pragma once
#include<functional>

class EventLoop;
class sock;
class InetAddress;
class Channel;
class Acceptor
{
private:
    EventLoop *loop;
    sock *Sock;
    InetAddress *addr;
    Channel *acceptChannel;
public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();
    void acceptConnection();
    std::function<void(sock*)> newConnectionCallback;
    void setNewConnectionCallback(std::function<void(sock*)>);
};
