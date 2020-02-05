#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstdlib>
#include <cstdio>
#include <error.h>
#include <sys/epoll.h>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include "Player.h"

const int one = 1;
std::vector<Player> players = std::vector<Player>();

int main(int argc, char ** argv){
    if(argc!=2)
        error(1,0,"Usage: %s <port>", argv[0]);

    sockaddr_in localAddress{
            .sin_family = AF_INET,
            .sin_port   = htons(atoi(argv[1])),
            .sin_addr   = {htonl(INADDR_ANY)}
    };

    int servSock = socket(PF_INET, SOCK_STREAM, 0);
    fcntl(servSock, F_SETFL, O_NONBLOCK, 1);
    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    if(bind(servSock, (sockaddr*) &localAddress, sizeof(localAddress)))
        error(1,errno,"Bind failed!");

    listen(servSock, 10);

    int fd = epoll_create1(0);

    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = servSock;

    epoll_ctl(fd, EPOLL_CTL_ADD, servSock, &event);

    int resultCount = epoll_wait(fd, &event, 1, -1);

    char buffer[1024];

    while (true)
    {
        if (event.events & EPOLLIN && event.data.fd == servSock)
        {
            int client_fd = accept(servSock, nullptr, nullptr);
            fcntl(client_fd, F_SETFL, O_NONBLOCK, 1);
            players.emplace_back(Player(client_fd, Position(0, 0)));
        }
        for (Player player : players)
        {
            int count = read(player.mFileDescriptor, buffer, sizeof(buffer) + 1);
            write(player.mFileDescriptor, buffer, count);
        }
        sleep(1);
    }
}
