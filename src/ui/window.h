/**
 * @file window.h
 * @brief Declares Window, the top-level application window.
 * @author Jaydee Brown (original window), Will Godderz (game setup + documentation), Trey Timko (Changed the game setup to allow for further customization, 2026-10-09)
 * @date 2026-09-17
 *
 * Window assembles the user interface: the InfoBar readout, the labelled
 * GridWidget, and a New Game button. It also runs the game-setup prompt that
 * asks the player for a mine count between 10 and 20, game mode, time limit, and AI settings
 * and connects the grid's status signals to the info bar.
 *
 * Inputs:  the player's mine-count choice, AI configuration, and gamemode configuration; Qt resize events.
 * Outputs: the assembled window; startNewGame() calls into GridWidget.
 *
 * External sources: mine-count prompt and signal wiring added with assistance
 * from Claude (Anthropic), a generative AI assistant, 2026-09-17.
 */
#ifndef WINDOW_H
#define WINDOW_H

#include "gridwidget.h"

#include <QMainWindow>
#include <QLabel>

class InfoBar;
class QPushButton;

// Trey Timko 10/09/2026
// A struct that contains information for how the game should be configured on start.
// This struct details the information the opening window will request for.
struct GameSettings {
    int mines; // Mine count, 10-20
    bool new_mode; // mode selection boolean for new mode
    int start_time; // the time limit in seconds
    AiMode aiMode; // Off, Interactive, Automatic
    AiDifficulty aiDifficulty; // Easy, Medium, or Hard
};

class Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // Opens the new-game dialog and restarts the game with the chosen settings.
    void promptNewGame();

private:
    void setupUi();

    // Trey Timko 10/09/2026
    // Modal dialog for the new-game options (mines, mode, AI).
    // Returns the player's choices.
    GameSettings askGameSettings();

    GridWidget *m_gridWidget = nullptr;
    InfoBar *m_infoBar = nullptr;
    QPushButton *m_newGameButton = nullptr;
};

#endif
