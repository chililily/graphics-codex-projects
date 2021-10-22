#pragma once

#include "Cell.h"

// FaceGrid
// Represents a square grid used to help construct "Pipe Cube" faces. Has "usable" space equal to (m_width - 1) * (m_width - 1) if perimeter blocking is applied, otherwise the usable space is (m_width)^2.
// 
// m_origin:    3D point representing the bottom left corner of the face (normal pointing outward)
// m_plane :    two-character string representing the plane the face occupies, e.g. "xz"
// m_grid  :    1D vector of Cells
// m_cursor:    Pointer to current active Cell
class FaceGrid
{
public:
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
    std::vector<Cell_ptr> m_grid;
    Cell_ptr m_cursor;

public:
    FaceGrid(const int width, const GridPlane plane, const Point3 origin, bool block_perimeter=true)
        : m_width{ width },
        m_plane { plane },
        m_origin{ origin }, 
        m_grid{ std::vector<Cell_ptr>(width * width) }
    {
        for (auto& cell_ptr : m_grid)
        {
            cell_ptr = std::make_shared<Cell>();
        }
        fillCellAdjacencies();
        if (block_perimeter) blockPerimeter();
    }

    // Initialization helpers
    void fillCellAdjacencies();
    void blockPerimeter();

    // Getters & setters
    Point3 origin() const { return m_origin; }
    auto size() const { return m_grid.size(); }
    Cell_ptr at (int grid_index);
    Cell_ptr at (const Point2& grid_position);
    Cell_ptr cursor() const { return m_cursor; }
    void setCursor(Cell_ptr cell_p) { m_cursor = cell_p; }
    void setCursor(int grid_index) { m_cursor = at(grid_index); }
    void setCursor(const Point2& grid_position) { m_cursor = at(grid_position); }
    void moveCursor(Direction dir) { m_cursor = m_cursor->adj(dir); }

    // (1D-2D) Index conversions
    int toIndex(const Point2& grid_position) { return grid_position.x + m_width * grid_position.y; }
    Point2 toCoordinates(int grid_index) { return Point2{ static_cast<float>(grid_index / m_width), static_cast<float>(grid_index % m_width) }; }

    Direction selectExtendDirection();
    Point3 worldPositionAt(const int grid_index);
    Point3 worldPositionAt(const Point2& grid_position);
    Point3 cursorWorldPosition();
};

typedef FaceGrid::GridPlane GridPlane;