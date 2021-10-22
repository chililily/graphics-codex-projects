#include "FaceGrid.h"

void FaceGrid::fillCellAdjacencies()
{
    // Fill Cell adjacency like so:
        // 0 1 3
        // 2 i 5
        // 4 6 7
    for (int i{ 0 }; i < m_grid.size(); ++i)
    {
        m_cursor = at(i);
        m_cursor->setIndex(i);

        // Not on top edge of grid
        if (i >= m_width)
        {
            m_cursor->setAdj(Direction::north, m_grid[i - m_width]);

            // Not on left edge
            if (i % m_width != 0) m_cursor->setAdj(Direction::northwest, m_grid[i - m_width - 1]);
            // Not on right edge
            if (i % m_width != m_width - 1) m_cursor->setAdj(Direction::northeast, m_grid[i - m_width + 1]);
        }

        // Not on left edge
        if (i % m_width != 0)
        {
            m_cursor->setAdj(Direction::west, m_grid[i - 1]);

            // Not on bottom edge
            if (size() - i > m_width) m_cursor->setAdj(Direction::southwest, m_grid[i + m_width - 1]);
        }

        // Not on right edge
        if (i % m_width != m_width - 1)
        {
            m_cursor->setAdj(Direction::east, m_grid[i + 1]);
            
            // Not on bottom edge;
            if (size() - i > m_width) m_cursor->setAdj(Direction::southeast, m_grid[i + m_width + 1]);
        }

        // Not on bottom edge
        if (size() - i > m_width)
        {
            m_cursor->setAdj(Direction::south, m_grid[i + m_width]);
        }
    }
}

// Fills perimeter of the FaceGrid
void FaceGrid::blockPerimeter()
{
    for (int i{ 0 }; i < m_grid.size(); ++i)
    {
        m_cursor = at(i);

        // On top or left edge of grid
        if (i < m_width || i % m_width == 0) m_cursor->fill();

        // On right or bottom edge
        if (i % m_width == m_width - 1 || size() - i <= m_width)
        {
            m_cursor->nullQuadCount();
            m_cursor->fill();
        }
    }
}

// Returns how much usable space (i.e. number of unoccupied cells at initialization) a FaceGrid has.
int FaceGrid::usable() const
{
    if (m_perimeter_blocking) return (m_width - 1) * (m_width - 1);
    else return size();
}

Cell_ptr FaceGrid::at(int grid_index)
{ 
    if (grid_index < size())
        return m_grid[grid_index];
    else 
        return nullptr;
}
Cell_ptr FaceGrid::at(const Point2& grid_position)
{ 
    if (0 <= grid_position.x && grid_position.x < m_width && 0 <= grid_position.y && grid_position.y < m_width)
    {
        int index{ toIndex(grid_position) };
        return m_grid[index];
    }
    else
        return nullptr;
}

// Returns a cardinal direction for cube extension
// Assumes cursor points to the cell of a recently placed cube
Direction FaceGrid::selectExtendDirection()
{
    static constexpr std::array<Direction, 4> cardinal{
        Direction::north,
        Direction::east,
        Direction::south,
        Direction::west
    };

    int start{ getRandInt(0, 3) };

    int i;
    for (int tries{0}; tries < 4; ++tries)
    {
        i = (start + tries) % cardinal.size();
        if (cursor()->adj(cardinal[i])->isAvailable()) return cardinal[i];
    }

    return Direction::max_directions;
}

Point3 FaceGrid::worldPositionAt(const int grid_index)
{
    Point2 grid_position{ toCoordinates(grid_index) };

    return worldPositionAt(grid_position);
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

Point3 FaceGrid::cursorWorldPosition()
{
    Point2 grid_position{ toCoordinates(m_cursor->index()) };

    return worldPositionAt(grid_position);
}