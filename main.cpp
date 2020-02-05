#include <unistd.h>
#include <errno.h>
#include <string>
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
#include <cstring>

const int one = 1;
std::vector<Player *> players = std::vector<Player *>();


int main(int argc, char ** argv) {
    if(argc!=2)
        error(1,0,"Usage: %s <port>", argv[0]);

    sockaddr_in localAddress {
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

    while (true) {
        if (event.events & EPOLLIN && event.data.fd == servSock) {
            int client_fd = accept(servSock, nullptr, nullptr);
            if (client_fd != -1) {
                fcntl(client_fd, F_SETFL, O_NONBLOCK, 1);
                players.emplace_back(new Player(client_fd, Position(0, 0)));
                write(players.back()->mFileDescriptor, "V ", 3);
            }
        }
        for (Player * player : players) {
            int count = read(player->mFileDescriptor, buffer, sizeof(buffer) + 1);
            if (count <= 0)
                continue;
            std::string message(buffer);
            if (!message.empty()){
                int position = message.find(" ");
                std::string xShift = message.substr(0, position);
                std::string yShift = message.substr(position + 1, message.size());
                player->move(std::stoi(xShift), std::stoi(yShift));
                write(player->mFileDescriptor, player->getCoordinates().data(), player->getCoordinatesLen());
                memset(buffer, 0, sizeof(buffer));
            }
        }
        sleep(1);
    }
}
