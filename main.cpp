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
#include <cstring>
#include "Player.h"
#include "Food.h"

const int one = 1;
std::vector<Player *> players = std::vector<Player *>();
std::vector<Food *> snacks = std::vector<Food *>();
std::string endMessage = std::string("end");
int boardWidth = 1000;
int boardLength = 1000;
char buffer[1024];
char sendingBuffer[1024];

void sendPositions(int fileDescriptor);

void addPlayer(int fileDescriptor);

bool isEaten(Player * player);

void eatFood(Player * player);

void spawnFood(int spawnRate);

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
    std::cout << resultCount << "\n";

    int xShift = 0;
    int yShift = 0;
    while (true) {
        if (event.events & EPOLLIN && event.data.fd == servSock) {
            int client_fd = accept(servSock, nullptr, nullptr);

            if (client_fd != -1) {
                fcntl(client_fd, F_SETFL, O_NONBLOCK, 1);
                addPlayer(client_fd);

                Player *currentPlayer = players.back();
                memcpy(&sendingBuffer[0], &currentPlayer->mId, sizeof(currentPlayer->mId));

                write(currentPlayer->mFileDescriptor, buffer, sizeof(currentPlayer->mId));
                memset(buffer, 0, sizeof(buffer));
            }
        }
        for (auto i = players.begin(); i != players.end(); i++) {
            if (*i == nullptr)
                break;
            Player *player = *i;
            int count = read(player->mFileDescriptor, buffer, sizeof(buffer) + 1);

            if (count <= 0) {
                player->timeToLive -= 1;
                if (player->timeToLive <= 0) {
                    close(player->mFileDescriptor);
                    delete player;
                    players.erase(i);
                    continue;
                }
            } else
                player->timeToLive = 10;

            int readIndex = 0;
            memcpy(&xShift, &buffer[readIndex], sizeof(xShift));
            readIndex += sizeof(xShift);
            memcpy(&yShift, &buffer[readIndex], sizeof(yShift));
            player->move(xShift, yShift);

            if (isEaten(player)) {
                player->mSize = 0;
            }

            eatFood(player);
            sendPositions(player->mFileDescriptor);
            memset(buffer, 0, sizeof(buffer));
        }
        if (snacks.size() > 1000)
            spawnFood(1);
    }
}

void sendPositions(int fileDescriptor) {
    int writeIndex;
    int xShift;
    int yShift;
    int empty = 0;
    bool end;
    for (Player *player : players) {
        writeIndex = 0;
        xShift = player->getX();
        yShift = player->getY();
        end = false;
        memcpy(&sendingBuffer[writeIndex], &player->mId, sizeof(player->mId));
        writeIndex += sizeof(player->mId);
        memcpy(&sendingBuffer[writeIndex], &xShift, sizeof(xShift));
        writeIndex += sizeof(xShift);
        memcpy(&sendingBuffer[writeIndex], &yShift, sizeof(yShift));
        writeIndex += sizeof(yShift);
        memcpy(&sendingBuffer[writeIndex], &player->mSize, sizeof(player->mSize));
        writeIndex += sizeof(player->mSize);
        memcpy(&sendingBuffer[writeIndex], &end, sizeof(end));
        writeIndex += sizeof(end);
        if (-1 == write(fileDescriptor, sendingBuffer, writeIndex)) {
            memset(sendingBuffer, 0, sizeof(sendingBuffer));
            return;
        }
        memset(sendingBuffer, 0, sizeof(sendingBuffer));
        usleep(15000);
    }
    writeIndex = 0;
    end = true;
    memcpy(&sendingBuffer[writeIndex], &empty, sizeof(empty));
    writeIndex += sizeof(empty);
    memcpy(&sendingBuffer[writeIndex], &empty, sizeof(empty));
    writeIndex += sizeof(empty);
    memcpy(&sendingBuffer[writeIndex], &empty, sizeof(empty));
    writeIndex += sizeof(empty);
    memcpy(&sendingBuffer[writeIndex], &empty, sizeof(empty));
    writeIndex += sizeof(empty);
    memcpy(&sendingBuffer[writeIndex], &end, sizeof(end));
    writeIndex += sizeof(end);
    if (-1 == write(fileDescriptor, sendingBuffer, writeIndex)) {
        memset(sendingBuffer, 0, sizeof(sendingBuffer));
        return;
    }
    memset(sendingBuffer, 0, sizeof(sendingBuffer));
    usleep(50);
}

void addPlayer(int fileDescriptor) {
    for (int id = 0; id < 10; id++) {
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

bool isEaten(Player * player) {
    for (Player * anotherPlayer : players) {
        if ((player->getX() - anotherPlayer->getX()) ^ 2 + (player->getY() - anotherPlayer->getY()) ^ 2 <= anotherPlayer->mSize ^ 2) {
            anotherPlayer->mSize += player->mSize;
            return true;
        }
    }
    return false;
}

void eatFood(Player * player) {
    for (Food * food : snacks) {
        if (((food->getX() - player->getX() ^ 2) + (food->getY() - player->getY()) ^ 2) <= player->mSize) {
            player->mSize += 1;
            delete food;
        }
    }
}

void spawnFood(int spawnRate) {
    // TODO: spawn food
}
