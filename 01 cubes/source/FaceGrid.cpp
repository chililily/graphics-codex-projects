#include "FaceGrid.h"

Point2 convert(Direction dir)
{
    switch (dir)
    {
    case Direction::dir_north:
        return Point2{ 0, 1 };
    case Direction::dir_south:
        return Point2{ 0, -1 };
    case Direction::dir_east:
        return Point2{ -1, 0 };
    case Direction::dir_west:
        return Point2{ 1, 0 };
    case Direction::dir_northeast:
        return Point2{ -1, 1 };
    case Direction::dir_southwest:
        return Point2{ 1, -1 };
    case Direction::dir_northwest:
        return Point2{ 1, 1 };
    case Direction::dir_southeast:
        return Point2{ -1, -1 };
    }
}

void DirectionFlagSet::block(Direction dir)
{
    auto i_dir{ static_cast<int>(dir) };
    m_flags[i_dir].block();

    switch (dir)
    {
    case Direction::dir_north:
    case Direction::dir_south:
    case Direction::dir_east:
    case Direction::dir_west:
        return;
    case Direction::dir_northeast:
    case Direction::dir_southwest:
    case Direction::dir_northwest:
    case Direction::dir_southeast:
        auto i_ccw{ static_cast<int>(DirectionFlag::counterclockwise(dir)) };
        auto i_cw{ static_cast<int>(DirectionFlag::clockwise(dir)) };
        m_flags[i_ccw].block();
        m_flags[i_cw].block();
        return;
    }
}

GridUnit FaceGrid::at(int x) const 
{ 
    if (x < size())
        return m_grid[x];
    else 
        return GridUnit::invalid;
}
GridUnit FaceGrid::at(const Point2& v) 
{ 
    if (0 <= v.x && v.x < m_width && 0 <= v.y && v.y < m_width)
    {
        int index{ toIndex(v) };
        return m_grid[index];
    }
    else
        return GridUnit::invalid;
}

void FaceGrid::selectDirections(const Point2& grid_position)
{
    m_flagset.reset();

    // Block a random cardinal direction
    m_flagset.block(static_cast<Direction>(getRandInt(0, 3)));

    // Check ordinal directions, if occupied block any adjacent cardinal directions
    for (auto flag : m_flagset.vector())
    {
        if (at(grid_position + convert(flag.dir())) == GridUnit::occupied)
        {
            flag.block();
        }
    }
}

Point2 FaceGrid::move(Point2& grid_position, Direction dir)
{
    grid_position += convert(dir);
    return grid_position;
}

Point3 FaceGrid::worldPositionAt(const Point2& grid_position)
{
    switch (m_plane)
    {
    case GridPlane::plane_xy:
        return Point3{ m_origin.x + grid_position.x, m_origin.y + grid_position.y, m_origin.z };
    case GridPlane::plane_xz:
        return Point3{ m_origin.x + grid_position.x, m_origin.y, m_origin.z + grid_position.y };
    case GridPlane::plane_yz:
        return Point3{ m_origin.x, m_origin.y + grid_position.x, m_origin.z + grid_position.y };
    }
}