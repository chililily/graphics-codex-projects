#pragma once

#include <G3D/G3D.h>
#include "utility.h"

class DirectionFlag
{
public:
    enum class Direction
    {
        dir_north,
        dir_south,
        dir_east,
        dir_west,
        dir_northeast,
        dir_southwest,
        dir_northwest,
        dir_southeast,

        max_directions
    };

    static Direction counterclockwise(Direction dir)
    {
        switch (dir)
        {
        case Direction::dir_north:
            return Direction::dir_northwest;
        case Direction::dir_south:
            return Direction::dir_southeast;
        case Direction::dir_east:
            return Direction::dir_northeast;
        case Direction::dir_west:
            return Direction::dir_southwest;
        case Direction::dir_northeast:
            return Direction::dir_north;
        case Direction::dir_southwest:
            return Direction::dir_south;
        case Direction::dir_northwest:
            return Direction::dir_west;
        case Direction::dir_southeast:
            return Direction::dir_east;
        }
    }

    static Direction clockwise(Direction dir)
    {
        switch (dir)
        {
        case Direction::dir_north:
            return Direction::dir_northeast;
        case Direction::dir_south:
            return Direction::dir_southwest;
        case Direction::dir_east:
            return Direction::dir_southeast;
        case Direction::dir_west:
            return Direction::dir_northwest;
        case Direction::dir_northeast:
            return Direction::dir_east;
        case Direction::dir_southwest:
            return Direction::dir_west;
        case Direction::dir_northwest:
            return Direction::dir_north;
        case Direction::dir_southeast:
            return Direction::dir_south;
        }
    }

private:
    bool m_blocked;
    Direction m_direction;

public:
    DirectionFlag(DirectionFlag::Direction dir)
        : m_blocked{ false }, m_direction{ dir }
    {}
    Direction dir() { return m_direction; }
    bool isBlocked() { return m_blocked; }
    void block() { m_blocked = true; }
    void reset() { m_blocked = false; }

    friend Point2 convert(Direction dir);
};

typedef DirectionFlag::Direction Direction;

class DirectionFlagSet
{
    std::vector<DirectionFlag> m_flags;

public:
    DirectionFlagSet() : m_flags{ {
            DirectionFlag{Direction::dir_north},
            DirectionFlag{Direction::dir_south},
            DirectionFlag{Direction::dir_east},
            DirectionFlag{Direction::dir_west},
            DirectionFlag{Direction::dir_northeast},
            DirectionFlag{Direction::dir_southwest},
            DirectionFlag{Direction::dir_northwest},
            DirectionFlag{Direction::dir_southeast} } }
    {}

    const std::vector<DirectionFlag>& vector() const { return m_flags; }
    auto getFlag(Direction dir) const { return m_flags[static_cast<int>(dir)]; }

    auto begin() const { return m_flags.begin(); }
    auto end() const { return m_flags.end(); }

    void block(Direction dir);
    void reset() { for (auto& flag : m_flags) flag.reset(); }
};

// FaceGrid
// Represents a square grid used to help construct "Fancy Cube" faces.
// 
// m_plane :    two-character string representing the plane the face occupies, e.g. "xz"
class FaceGrid
{
public:
    enum class GridUnit
    {
        empty,
        occupied,
        invalid
    };

    enum class GridPlane
    {
        plane_xy,
        plane_xz,
        plane_yz
    };

private:
    int m_width;
    Point3 m_origin;
    GridPlane m_plane;
    std::vector<GridUnit> m_grid;
    DirectionFlagSet m_flagset;

public:
    FaceGrid(const int width, const GridPlane plane, const Point3 origin)
        : m_plane{ plane }, m_origin{ origin }, m_flagset{ DirectionFlagSet{} }
    {
        if (width <= 0) throw "FaceGrid: width must be positive integer";
        m_width = width;
        m_grid = std::vector<GridUnit>(width * width, GridUnit::empty);
    }

    // Getters & setters
    GridUnit at(int x) const;
    GridUnit at(const Point2& v);
    Point3 origin() const { return m_origin; }
    auto size() const { return m_grid.size(); }
    DirectionFlagSet& flagset() { return m_flagset; }
    void fill(int x) { m_grid[x] = GridUnit::occupied; }
    void fill(const Point2& v) { m_grid[v.x + m_width * v.y] = GridUnit::occupied; }

    // Conversions
    int toIndex(const Point2& coords) { return coords.x + m_width * coords.y; }
    Point2 toCoordinates(int x) { return Point2{ static_cast<float>(x / m_width), static_cast<float>(x % m_width) }; }

    void selectDirections(const Point2& grid_position);
    Point2 move(Point2& grid_position, Direction dir);
    Point3 worldPositionAt(const Point2& grid_position);
};

typedef FaceGrid::GridUnit GridUnit;
typedef FaceGrid::GridPlane GridPlane;