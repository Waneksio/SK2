//
// Created by matt on 05.02.20.
//

#ifndef SK2_ENTITY_H
#define SK2_ENTITY_H


#include "Position.h"

class Entity {
public:
    Position mPosition;
    Entity(Position position) : mPosition(position) {};
};


#endif //SK2_ENTITY_H
