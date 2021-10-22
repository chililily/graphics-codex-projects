#include "Direction.h"

Direction antiparallel(Direction dir)
{
    switch (dir)
    {
    case Direction::north:
        return Direction::south;
    case Direction::south:
        return Direction::north;
    case Direction::east:
        return Direction::west;
    case Direction::west:
        return Direction::east;
    case Direction::northeast:
        return Direction::southwest;
    case Direction::southwest:
        return Direction::northeast;
    case Direction::northwest:
        return Direction::southeast;
    case Direction::southeast:
        return Direction::northwest;
    }
}