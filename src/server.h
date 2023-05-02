#pragma once

class EventLoop;
class sock;
class Acceptor;
class Server
{
private:
    EventLoop *loop;
    Acceptor *acceptor;
public:
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);
    void newConnection(sock *ser_sock);
};
