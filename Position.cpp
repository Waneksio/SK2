//
// Created by matt on 05.02.20.
//

#include "Position.h"

float Position::getDistance(Position position) {
    int xDistance = mx - position.mx;
    int yDistance = my - position.my;
    return sqrt(xDistance ^ 2 + yDistance ^ 2);
}

Position::Position(int x, int y) {
    mx = x;
    my = y;
}
