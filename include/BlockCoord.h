#ifndef BLOCKCOORD_H
#define BLOCKCOORD_H

#include <mcpp/mcpp.h>

class BlockCoord {
private:
    mcpp::BlockType block2;
    mcpp::Coordinate coord2;

public:
    BlockCoord(mcpp::BlockType block, mcpp::Coordinate coord)
        : block2(block), coord2(coord) {}

    mcpp::BlockType getBlock2() const { return block2; }

    mcpp::Coordinate getCoord2() const { return coord2; }
};

#endif
