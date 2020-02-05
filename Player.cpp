//
// Created by matt on 05.02.20.
//

#include "Player.h"

Player::Player(int fd, Position position) : Entity(position) {
    mFileDescriptor = fd;
    mSize = 1;
}

void Player::move(int xShift, int yShift) {
    mPosition.mx += xShift;
    mPosition.my += yShift;
}
