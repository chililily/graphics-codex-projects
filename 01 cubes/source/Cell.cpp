#include "Cell.h"

// Returns pointer to adjacent Cell in specified Direction
Cell_ptr Cell::adj(Direction dir)
{
    assert(dir != Direction::max_directions);
    return m_adj[(int)dir];
}

bool Cell::isAvailable() const 
{ 
    if (this == nullptr) return false; 
    return (m_state == CellState::available); 
}

// Returns true on success
// Returns false if cell was not available
bool Cell::fill()
{
    if (m_state == CellState::available)
    {
        m_state = CellState::occupied;
        updateQuads();
        return true;
    }
    return false;
}

void Cell::updateQuads()
{
    // Update self quad count
    updateQuad();

    // Update adjacent cells' quad counts
    m_adj[(int)Direction::north]->updateQuad();
    m_adj[(int)Direction::west]->updateQuad();
    m_adj[(int)Direction::northwest]->updateQuad();
}

// Increments the count of occupied Cells in a valid quad, and blocks the remaining cell in quads that accumulate 3 occupied Cells.
void Cell::updateQuad()
{
    if (this == nullptr) return;
    if (m_quadcount < 0) return;

    ++m_quadcount;

    if (m_quadcount == 3)
    {
        if (this->isAvailable())
        {
            this->block();
            return;
        }

        Cell_ptr cell;
        for (int i{ 0 }; i < 3; ++i)
        {
            cell = m_adj[i+5];
            if (cell->isAvailable())
            {
                cell->block();
                return;
            }
        }
    }
}