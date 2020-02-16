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

void sendPositions(int fileDescriptor);

void addPlayer(int fileDescriptor);

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
                addPlayer(client_fd);
                Player * currentPlayer = players.back();
                write(currentPlayer->mFileDescriptor, currentPlayer->getId().data(), currentPlayer->getIdLen());
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
                if (xShift.length() == 0 || yShift.length() == 0)
                    player->move(10, 10);
                else
                    player->move(std::stoi(xShift), std::stoi(yShift));
                sendPositions(player->mFileDescriptor);
                memset(buffer, 0, sizeof(buffer));
            }
        }
        sleep(1);
    }
}

void sendPositions(int fileDescriptor) {
    for (Player * player : players) {
        std::string message = player->getId().data();
        message.append(" ");
        message.append(player->getCoordinates().data());
        write(fileDescriptor, message.data(), sizeof(message.data()));
    }
    write(fileDescriptor, "end", 3);
}

void addPlayer(int fileDescriptor) {
    for (int id = 1; id < 11; id++) {
        bool skip = false;
        for (Player * player : players) {
            if (player->mId == id) {
                skip = true;
                break;
            }
        }
        if (skip)
            continue;
        players.emplace_back(new Player(fileDescriptor, Position(0, 0), id));
        break;
    }
}
