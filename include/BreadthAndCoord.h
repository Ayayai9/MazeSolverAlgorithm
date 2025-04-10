#ifndef BREADTHANDCOORD_H
#define BREADTHANDCOORD_H

#include <mcpp/mcpp.h>

class BreadthAndCoord {
private:
    int breadth;
    mcpp::Coordinate coord;

public:
    BreadthAndCoord(int breadth, mcpp::Coordinate coord)
        : breadth(breadth), coord(coord) {}

    bool operator==(const BreadthAndCoord &other) const {
        return breadth == other.breadth && coord == other.coord;
    }

    int getBreadth() const { return breadth; }

    mcpp::Coordinate getCoord() const { return coord; }
};

#endif
