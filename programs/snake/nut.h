#ifndef SNAKE_NUT_H
#define SNAKE_NUT_H

#include "node.h"

class Nut
{
public:
    Nut(int inMatrixWidth, int inMatrixHeight);
    void regenerate();
    int getX() const { return pos.x; }
    int getY() const { return pos.y; }
    const Node& getNode() { return pos; }

private:
    int matrixWidth, matrixHeight;
    Node pos;
};

#endif
