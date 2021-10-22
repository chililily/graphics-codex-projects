#pragma once

#include "utility.h"

enum class Direction
{
    north = 1,
    south = 6,
    east = 5,
    west = 2,
    northeast = 3,
    southwest = 4,
    northwest = 0,
    southeast = 7,

    max_directions
};

Direction antiparallel(Direction dir);