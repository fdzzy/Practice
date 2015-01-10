#ifndef SNAKE_NODE_H
#define SNAKE_NODE_H

struct Node
{
    Node(int inX = 0, int inY = 0) : x(inX), y(inY) {}
    Node(const Node &aNode) : x(aNode.x), y(aNode.y) {}
    Node& operator=(const Node &aNode) {
        if(&aNode != this) {
            x = aNode.x;
            y = aNode.y;
        }
        return *this;
    }
    int x,y;
};

inline bool operator==(const Node &a, const Node &b)
{
    return ( (a.x == b.x) && (a.y == b.y));
}

inline bool operator!=(const Node &a, const Node &b)
{
    return !(a == b);
}

#endif
