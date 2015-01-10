#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include "debug.h"
#include "node.h"

#define Initial_length 8 
#define Initial_x(width) (width * 0.2)
#define Initial_y(height) (height * 0.2)

enum Direction { Up, Down, Left, Right };

class Snake
{
public:
    Snake(int inMatrixWidth, int inMatrixHeight);
    Snake(int inMatrixWidth, int inMatrixHeight, int initialx, int initialy, int initialLength);

    bool isAlive() const;
    const std::vector<Node>& getNodes() const;
    bool isBody(const Node &node) const;

    Node* move(Direction dir, bool extend = false);
    void restart();

private:
    void makeSnake(int x, int y, int length);

    bool alive;
    int matrixWidth, matrixHeight;
    std::vector<Node> nodes;
};

#endif
