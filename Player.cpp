//
// Created by matt on 05.02.20.
//

#include "Player.h"

Player::Player(int fd, Position position, int id) : Entity(position) {
    mFileDescriptor = fd;
    mSize = 1;
    mId = id;
}

void Player::move(int xShift, int yShift) {
    mPosition.mx += xShift;
    mPosition.my += yShift;
}

std::string Player::getCoordinates() {
    std::string coordinates = std::string();
    std::string xCoordinate = std::to_string(mPosition.mx);
    std::string yCoordinate = std::to_string(mPosition.my);
    coordinates.append(xCoordinate);
    coordinates.append(" ");
    coordinates.append(yCoordinate);
    coordinates.append(" ");
    coordinates.append(std::to_string(mSize));
    return coordinates;
}

std::string Player::getId() {
    std::string result = std::to_string(mId);
    return result;
}