//
// Created by matt on 05.02.20.
//

#ifndef SK2_PLAYER_H
#define SK2_PLAYER_H

#include "Position.h"
#include "Entity.h"
#include <string>


class Player : public Entity{
public:
    int mFileDescriptor;
    int mSize;
    int mId;
    Player(int fd, Position position, int id);
    void move(int xShift, int yShift);
    std::string getCoordinates();
};


#endif //SK2_PLAYER_H