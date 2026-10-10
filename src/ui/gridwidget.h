// File: Cgridwidget.h
// Project: EECS 581 - Project 1: Minesweeper
// Description: Header definition for GridWidget, declaring the 10x10board layout, title matrix storage, 
//              and Qt signals/slots for title click routing.
// Author: Jaydee Brown, Will Godderz
// Creation Date: 2026-09-17

/**
 * @file gridwidget.h
 * @brief Declares GridWidget, the Input Handler and grid renderer.
 * @author Jaydee Brown (original grid), Will Godderz (game wiring + documentation)
 * @date 2026-09-17
 *
 * GridWidget draws the labelled 10x10 board (columns A-J, rows 1-10) and acts
 * as the Input Handler in the system architecture: it receives tile clicks,
 * forwards them to LogicHandler, then repaints only the cells the logic reports
 * as changed. It owns the Board and the LogicHandler for the current game.
 *
 * Inputs:  tileClick signals from Tile; startNewGame(numMines) from Window.
 * Outputs: repainted tiles, plus flagsRemainingChanged(int) and
 *          statusChanged(GameState) signals consumed by the InfoBar.
 *
 * External sources: game wiring added with assistance from Claude (Anthropic),
 * a generative AI assistant, 2026-09-17 and 2026-10-10.
 */
#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

#include "board.h"
#include "logicHandler.h"

#include <QWidget>
#include <QVector>
#include <QTimer>

class Tile;
// John Platkowski 10/10/2026 migrate AI enums from window class
enum class AiMode { Off, Interactive, Automatic };
enum class AiDifficulty { Easy, Medium, Hard };

class GridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GridWidget(QWidget *parent = nullptr);

    static constexpr int kGridSize = 10;

    // Clears the board and begins a new game with the given mine count (10-20).
    void startNewGame(int numMines, bool timed = false, int start_time = 120, AiMode aiMode = AiMode::Off, AiDifficulty aiDifficulty = AiDifficulty::Easy);

signals:
    // Emitted whenever a flag is placed or removed (total mines - flags placed).
    void flagsRemainingChanged(int remaining);

    // Emitted when the game transitions between Playing / Won / Lost.
    void statusChanged(GameState state);

    void timeChanged(int seconds);

private slots:
    void handleTileClicked(Tile *tile, Qt::MouseButton clickType);
    void onTimeChanged(int seconds);

    // Plays one AI move, then re-arms itself in Automatic mode.
    void playAiMove();

private:
    void setupUi();

    // Sends one click to the Game Logic, repaints changed cells
    bool applyClick(int row, int col, ClickType click);

    // Reads one cell from the Board and pushes its appearance into its Tile.
    void refreshTile(int row, int col);

    // Repaints every tile from the current Board state (used on new game).
    void refreshAllTiles();

    QVector<QVector<Tile*>> m_tiles;

    // Declaration order matters: m_board must outlive and precede m_logic,
    // because m_logic binds a reference to it in the constructor's init list.
    Board m_board;
    LogicHandler m_logic;
    bool m_timedMode = false; 

    // Delay before each AI move
    static constexpr int kAiMoveDelayMs = 400;

    AiMode m_aiMode = AiMode::Off;
    AiDifficulty m_aiDifficulty = AiDifficulty::Easy;
    // One shot, precludes player input while active
    QTimer m_aiTimer;
};

#endif
