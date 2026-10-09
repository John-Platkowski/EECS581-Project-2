/**
 * @file logicHandler.cpp
 * @brief Implementation of the Game Logic component for Minesweeper.
 * @author Will Godderz
 * @date 2026-09-16
 *
 * Implements every gameplay rule declared in logicHandler.h: deferred mine
 * placement with a guaranteed-safe first click, adjacency counting, flag
 * toggling, recursive reveal of zero-adjacency regions, loss handling and win detection.
 *
 * Inputs:  click coordinates and button type, applied against the bound Board.
 * Outputs: lists of changed cell coordinates for the UI to repaint, and the
 *          updated GameState.
 *
 * External sources:
 *   - syntax logic written with assistance from Claude (Anthropic), a
 *     generative AI assistant, on 2026-09-16.
 */
#include "logicHandler.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>

// the constructor, receives the board by reference
LogicHandler::LogicHandler(Board& board)
    : board(board),
      m_state(GameState::Playing), // game begins in the Playing state
      m_minesPlaced(false), // mines not placed yet; first click will place them
      m_rng(static_cast<unsigned int>( // seed the random generator
          std::chrono::steady_clock::now().time_since_epoch().count()))
{
}

// rebuild the board for a new game
void LogicHandler::reset(int numMines) {
    board = Board(numMines);
    m_state = GameState::Playing;
    m_minesPlaced = false;

    m_gameTimer.reset();
}

// how many flags the player still has to place
int LogicHandler::flagsRemaining() const
{
    return board.getTotalMines() - board.getTotalFlags(); // total mines minus flags already placed
}

// entry point for all player input. Returns the cells the UI must repaint.
CellList LogicHandler::handleClick(int row, int col, ClickType clickType)
{

    // Jack Larson
    // 10/8/2026
    // dummy code that replaces user clicks with algo clicks :)
    // std::tuple<int, int, ClickType> algo3click = difficultyThreeBestMove();
    // row = std::get<0>(algo3click);
    // col = std::get<1>(algo3click);
    // clickType = std::get<2>(algo3click);

    CellList changedCells; // list of cells we touched, starts empty

    if (m_state != GameState::Playing)
    { // if the game is already won or lost
        return changedCells;
    }

    if (!board.isValidCoordinate(row, col))
    { // if the click is outside the grid
        return changedCells;
    }

    if (clickType == ClickType::Right)
    {                                               // right click
        const Cell &cell = board.getCell(row, col); // grab a read-only reference to the clicked cell
        if (cell.isCovered() || cell.isFlagged())
        {                                        // only covered/flagged cells can be flagged or un-flagged
            board.toggleFlag(row, col);          // flip the flag on/off
            changedCells.emplace_back(row, col); // mark this cell as repaint
        }
        return changedCells;
    }

    if (board.getCell(row, col).isFlagged())
    { // left-clicking an already flagged cell do nothing
        return changedCells;
    }

    if (!board.getCell(row, col).isCovered())
    { // left click on an already-uncovered cell does nothing
        return changedCells;
    }

    // first left click of the game decides the mine layout, so that this cell and its neighbours are guaranteed clear.
    if (!m_minesPlaced)
    {
        placeMines(row, col);
        computeAdjacency(); // now that mines exist, compute each cell's 0-8 number
        m_minesPlaced = true;
        // Start timer when the first cell is clicked
        m_gameTimer.start();
    }

    // re-read the cell placement above may have set its adjacency count.
    const Cell &cell = board.getCell(row, col);

    if (cell.hasMine)
    {                                // if the clicked cell is a mine
        board.uncoverCell(row, col); // uncover it
        changedCells.emplace_back(row, col);
        m_state = GameState::Lost; // mark it for repaint
        m_gameTimer.stop();

        revealAllMines(changedCells); // expose every other mine for the game-over view
        return changedCells;
    }

    if (board.uncoverCell(row, col)) {
        changedCells.emplace_back(row, col); // try to uncover the safe cell
    }

    // a cell with no adjacent mines opens up its whole connected region.
    if (cell.numOfAdjacentMines == 0) {
        revealConnectedCells(row, col, changedCells);
    }

    // any successful uncover may have completed the board.
    if (allSafeCellsUncovered()) {
        m_state = GameState::Won;
        m_gameTimer.stop();
    }
    return changedCells;

}

// place mines, avoiding the first-clicked cell and its neighbors
void LogicHandler::placeMines(int safeRow, int safeCol) { 
    std::vector<int> candidates;
    candidates.reserve(Board::ROWS * Board::COLL); // list of cells where a mine is allowed to go

    for (int r = 0; r < Board::ROWS; ++r) { // loop over every row and column
        for (int c = 0; c < Board::COLL; ++c) {
            const bool nearFirstClick = std::abs(r - safeRow) <= 1 && std::abs(c - safeCol) <= 1; // is this cell in the 3x3 zone around the first click
            if (nearFirstClick) {
                continue;
            }
            candidates.push_back(r * Board::COLL + c); // otherwise this cell is legal
        }
    }

    std::shuffle(candidates.begin(), candidates.end(), m_rng); // randomly reorder all legal cells

    const int mineCount = std::min<int>(board.getTotalMines(), static_cast<int>(candidates.size())); // how many mines to actually place

    for (int i = 0; i < mineCount; ++i) { // place a mine at each of the first mineCount shuffled cells, converting the flat index back to (row, col) using / and % COLL
        board.placeMine(candidates[i] / Board::COLL, candidates[i] % Board::COLL);
    }
}

// after placement record how many mines touch each cell so the UI can show 0-8.
void LogicHandler::computeAdjacency() {
    for (int r = 0; r < Board::ROWS; ++r) {
        for (int c = 0; c < Board::COLL; ++c) {
            board.setNumAdjMine(r, c, countAdjacentMines(r, c));
        }
    }
}

// count mines among the eight neighbours of (row, col), skipping the cell itself and any coordinate that falls off the grid.
int LogicHandler::countAdjacentMines(int row, int col) const {
    int count = 0;
    for (int rowOffset = -1; rowOffset <= 1; ++rowOffset) { // step through row offsets: -1 (above), 0 (same), +1 (below)
        for (int colOffset = -1; colOffset <= 1; ++colOffset) { // step through col offsets: -1 (left), 0 (same), +1 (right)
            if (rowOffset == 0 && colOffset == 0) {
                continue;
            }
            const int neighborRow = row + rowOffset;
            const int neighborCol = col + colOffset;
            if (!board.isValidCoordinate(neighborRow, neighborCol)) { // if the neighbor is off the grid
                continue;
            }
            if (board.getCell(neighborRow, neighborCol).hasMine) { // if this valid neighbor has a mine add it
                ++count;
            }
        }
    }

    return count;
}


void LogicHandler::revealConnectedCells(int row, int col, CellList& changedCells) { // open up the empty region spreading out from (row, col)
    for (int rowOffset = -1; rowOffset <= 1; ++rowOffset) { // visit each neighbor
        for (int colOffset = -1; colOffset <= 1; ++colOffset) {
            if (rowOffset == 0 && colOffset == 0) {
                continue;
            }
            const int neighborRow = row + rowOffset;
            const int neighborCol = col + colOffset;
            if (!board.isValidCoordinate(neighborRow, neighborCol)) { // off the grid
                continue;
            }
            if (board.getCell(neighborRow, neighborCol).hasMine) { // never auto-open a mine
                continue; 
            }
            if (!board.uncoverCell(neighborRow, neighborCol)) { // try to uncover but fails if already open or flagged
                continue;
            }
            changedCells.emplace_back(neighborRow, neighborCol); // it uncovered, so mark it for repaint
            if (board.getCell(neighborRow, neighborCol).numOfAdjacentMines == 0) {
                revealConnectedCells(neighborRow, neighborCol, changedCells);
            }
        }
    }
}

// Loss handling expose every mine, clearing any flag first so the cell can actually be uncovered
void LogicHandler::revealAllMines(CellList& changedCells) {
    for (int r = 0; r < Board::ROWS; ++r) { // scan every row and column
        for (int c = 0; c < Board::COLL; ++c) {
            if (!board.getCell(r, c).hasMine) { // if this cell is NOT a mine skip 
                continue;
            }
            // a flagged mine must be unflagged before it will uncover.
            if (board.getCell(r, c).isFlagged()) {
                board.toggleFlag(r, c);
            }

            if (board.uncoverCell(r, c)) { // uncover it
                changedCells.emplace_back(r, c); // mark as repaint
            }
        }
    }
}

// Victory!!!!!!! condition every cell without a mine has been uncovered. 
bool LogicHandler::allSafeCellsUncovered() const {
    for (int r = 0; r < Board::ROWS; ++r) {
        for (int c = 0; c < Board::COLL; ++c) {
            const Cell& cell = board.getCell(r, c);
            if (!cell.hasMine && !cell.isUncovered()) {
                return false;
            }
        }
    }
    return true;
}

// Jack Larson 10/8/2026
// quick implementation, will probably be replaced when algo 2 is done
int LogicHandler::getAdjacentCoveredCount(int row, int col) const
{
    int count = 0;
    for (int x = row - 1; x < row + 2; x++)
    {
        for (int y = col - 1; y < col + 2; y++)
        {
            if (board.isValidCoordinate(x, y))
            {
                if (!board.getCell(x, y).isUncovered())
                    count++;
            }
        }
    }
    return count;
}

// Jack Larson 10/8/2026
// quick implementation, will probably be replaced when algo 2 is done
std::pair<int, int> LogicHandler::getFirstAdjacentCoveredUnflagged(int row, int col) const
{
    for (int x = row - 1; x < row + 2; x++)
    {
        for (int y = col - 1; y < col + 2; y++)
        {
            if (board.isValidCoordinate(x, y))
            {
                if (board.getCell(x, y).isCovered())
                    return std::pair<int, int>(x, y);
            }
        }
    }
    return std::pair<int, int>(-1, -1);
}

// Jack Larson 10/8/2026
// quick implementation, will probably be replaced when algo 2 is done
int LogicHandler::getAdjacentFlaggedCount(int row, int col) const
{
    int count = 0;
    for (int x = row - 1; x < row + 2; x++)
    {
        for (int y = col - 1; y < col + 2; y++)
        {
            if (board.isValidCoordinate(x, y) && board.getCell(x, y).isFlagged())
                count++;
        }
    }
    return count;
}

// Jack Larson 10/8/2026
// returns the target to click to solve 1 2 1 condition
std::pair<int, int> LogicHandler::targetFromOneTwoOne() const
{
    return std::pair<int, int>();
}

// Jack Larson 10/8/2026
// returns the best move as per algorithm 3
std::tuple<int, int, ClickType> LogicHandler::difficultyThreeBestMove() const
{
    for (int x = 0; x < Board::ROWS; x++)
    {
        for (int y = 0; y < Board::COLL; y++)
        {
            if (!board.getCell(x, y).isUncovered())
                continue;

            if (getAdjacentCoveredCount(x, y) == board.getCell(x, y).numOfAdjacentMines)
            {
                std::pair<int, int> firstCoveredUnflagged = getFirstAdjacentCoveredUnflagged(x, y);
                if (firstCoveredUnflagged != std::pair<int, int>(-1, -1))
                    return std::tuple<int, int, ClickType>(firstCoveredUnflagged.first, firstCoveredUnflagged.second, ClickType::Right);
            }

            if (getAdjacentFlaggedCount(x, y) == board.getCell(x, y).numOfAdjacentMines)
            {
                std::pair<int, int> firstCoveredUnflagged = getFirstAdjacentCoveredUnflagged(x, y);
                if (firstCoveredUnflagged != std::pair<int, int>(-1, -1))
                    return std::tuple<int, int, ClickType>(firstCoveredUnflagged.first, firstCoveredUnflagged.second, ClickType::Left);
            }

            if (board.getCell(x, y).numOfAdjacentMines == 2 && getAdjacentFlaggedCount(x, y) != 2)
            {
                if (x == 0 || x == Board::ROWS - 1 || y == 0 || y == Board::COLL - 1)
                    continue;

                bool xClear = board.getCell(x - 1, y).isUncovered() && board.getCell(x + 1, y).isUncovered();
                bool xOnes = board.getCell(x - 1, y).numOfAdjacentMines == 1 && board.getCell(x + 1, y).numOfAdjacentMines == 1;
                bool yClear = board.getCell(x, y - 1).isUncovered() && board.getCell(x, y + 1).isUncovered();
                bool yOnes = board.getCell(x, y - 1).numOfAdjacentMines == 1 && board.getCell(x, y + 1).numOfAdjacentMines == 1;
                if (xClear && xOnes || yClear && yOnes)
                {
                    if (board.getCell(x - 1, y - 1).isCovered())
                        return std::tuple<int, int, ClickType>(x - 1, y - 1, ClickType::Right);
                    if (board.getCell(x + 1, y - 1).isCovered())
                        return std::tuple<int, int, ClickType>(x + 1, y - 1, ClickType::Right);
                    if (board.getCell(x + 1, y + 1).isCovered())
                        return std::tuple<int, int, ClickType>(x + 1, y + 1, ClickType::Right);
                    if (board.getCell(x - 1, y + 1).isCovered())
                        return std::tuple<int, int, ClickType>(x - 1, y + 1, ClickType::Right);
                }
            }
        }
    }

    return std::tuple<int, int, ClickType>(0, 0, ClickType::Left);
}

// 10/6/2026 Tyler Oswald
// This function is used by the UI to get the current time
GameTimer *LogicHandler::getGameTimer()
{
    return &m_gameTimer;
}

// Wrote with the help of ChatGPT (Open AI) - Tyler 10/6/2026
// This function ends the game when time runs out in the timed mode
void LogicHandler::timeExpired()
{
    // If the game is already over, we don't need to run this again
    if (m_state != GameState::Playing)
    {
        return;
    }

    // Update the game state to reflect the loss
    m_state = GameState::Lost;
    // Stop the timer
    m_gameTimer.stop();

    // Make an empty cellist object to pass to revealAllMines
    CellList ignored;
    // Display the remaining mines on game loss
    revealAllMines(ignored);
}

// Tyler - 10/6/2026
// This function sets the countdown time in the timer class
void LogicHandler::setTimeLimit(bool timed, int seconds)
{
    // Set the time for the timer
    m_gameTimer.setCountdown(timed, seconds);
}
