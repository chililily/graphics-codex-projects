#pragma once

#include "Direction.h"
#include "utility.h"

class Cell;

typedef std::shared_ptr<Cell> Cell_ptr;

// Cell
//
// Object used to represent a grid unit within FaceGrid.
// Each Cell keeps track of pointers to adjacent Cells (if existing) in m_adj.
// Each Cell is also associated with at most one "quad," which is an abstract grouping of 4 cells in a 2X2 configuration, with the associated Cell in the northwest corner (i.e. a quad is managed by the Cell in its northwest corner).
class Cell
{
public:
    enum class CellState
    {
        available,
        occupied,
        blocked
    };

private:
    CellState m_state;
    std::array<Cell_ptr, 8> m_adj;
    int m_quadcount;
    int m_index;

public:
    Cell() : m_state{ CellState::available },
        m_adj{ std::array<Cell_ptr,8>{} },
        m_quadcount{ 0 }
    {}

    // Getters & setters
    Cell_ptr adj(Direction dir);
    CellState state() const { return m_state; }
    int index() const { return m_index; }
    bool isAvailable() const;
    void setIndex(int i) { m_index = i; }
    void setAdj(Direction dir, Cell_ptr cell_p) { m_adj[(int)dir] = cell_p; }
    void nullQuadCount() { m_quadcount = -1; }
    void block() { m_state = CellState::blocked; }
    bool fill();

    void updateQuads();
    void updateQuad();

    friend class FaceGrid;
};

typedef Cell::CellState CellState;