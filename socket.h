#ifndef SOCKET_H
#define SOCKET_H

class InetAddress;
class sock
{
    private:
        int fd;
    public:
        sock();
        sock(int);
        ~sock();

        void bind(InetAddress*);
        void listen();
        void setnonblocking();

        int accept(InetAddress*);

        int getFd();
};

#endif
