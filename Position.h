//
// Created by matt on 05.02.20.
//

#ifndef SK2_POSITION_H
#define SK2_POSITION_H

#include <cmath>


class Position {
public:
    int mx;
    int my;
    float getDistance(Position position);
    Position(int x, int y);
};


#endif //SK2_POSITION_H