//
// Created by matt on 05.02.20.
//

#ifndef SK2_PLAYER_H
#define SK2_PLAYER_H

#include "Position.h"
#include "Entity.h"


class Player : Entity{
public:
    int mFileDescriptor;
    int mSize;
    Player(int fd, Position position);
    void move(int xShift, int yShift);
};


#endif //SK2_PLAYER_H
