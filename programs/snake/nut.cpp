#include "nut.h"
#include <stdlib.h>
#include <time.h>

Nut::Nut(int inMatrixWidth, int inMatrixHeight)
    : matrixWidth(inMatrixWidth), matrixHeight(inMatrixHeight) {
    srand(time(NULL));
    regenerate(); 
}

void Nut::regenerate()
{
    pos.x = rand() % matrixWidth;
    pos.y = rand() % matrixHeight;
}

