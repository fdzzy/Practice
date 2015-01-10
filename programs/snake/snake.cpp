#include "snake.h"
#include <cstddef>

Snake::Snake(int inMatrixWidth, int inMatrixHeight)
{
    *this = Snake(inMatrixWidth, inMatrixHeight, Initial_x(inMatrixWidth), Initial_y(inMatrixHeight), Initial_length);
}

Snake::Snake(int inMatrixWidth, int inMatrixHeight, int initialx, int initialy, int initialLength)
    : matrixWidth(inMatrixWidth), matrixHeight(inMatrixHeight)
{
    alive = true;
    makeSnake(initialx, initialy, initialLength);
}

/*
* Args:
*   x:  initial x position
*   y:  initial y position
*   length: initial length
*/
void Snake::makeSnake(int x, int y, int length)
{
    int i;
    nodes.clear();
    /* Round length to x */
    if(length > x)
        length = x;
    for(i=0; i < length ; i++){
        nodes.push_back(Node(x - i, y)); 
    }
}

void Snake::restart()
{
    *this = Snake(matrixWidth, matrixHeight, Initial_x(matrixWidth), Initial_y(matrixHeight), Initial_length);
}

bool Snake::isBody(const Node &node) const
{
    for(int i=0; i < nodes.size() ; i++) {
        if(node == nodes[i])
            return true;
    }
    return false;
}

bool Snake::isAlive() const
{
    return alive;
}

const std::vector<Node>& Snake::getNodes() const
{
    return nodes;
}

/*
* Args:
*   dir: the direction to move
*   extend: if the Snake need to extend its length, default to false
* Return value:
*   Node*: the pointer to the new head of the snake, could be NULL if snake doesn't have any node or snake is dead
*/
Node* Snake::move(Direction dir, bool extend)
{
    int i;
    /* do nothing if nodes hasn't been set up */
    if(!nodes.size())
        return NULL;
    /* if we need to extend the snake length, save the last node for now */
    Node lastNode;
    lastNode = nodes[nodes.size()-1];
    for(i = (nodes.size()-1); i > 0 ; i--) {
        nodes[i].x = nodes[i-1].x;
        nodes[i].y = nodes[i-1].y;
    }
    Node newNode(nodes[0]);
    switch(dir) {
    case Up:
        newNode.y--;
        break;
    case Down:
        newNode.y++;
        break;
    case Left:
        newNode.x--;
        break;
    case Right: 
        newNode.x++;
        break;
    }
    if(isBody(newNode)) {
        alive = false;
        return NULL;
    }
    if(newNode.x < 0 || newNode.x >= matrixWidth || newNode.y < 0 || newNode.y >= matrixHeight) {
        alive = false;
        return NULL;
    }
    nodes[0] = newNode;
    /* save the last node now */
    if(extend) {
        nodes.push_back(lastNode);
    }
    return &nodes[0];
}

