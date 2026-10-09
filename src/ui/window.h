/**
 * @file window.h
 * @brief Declares Window, the top-level application window.
 * @author Jaydee Brown (original window), Will Godderz (game setup + documentation)
 * @date 2026-09-17
 *
 * Window assembles the user interface: the InfoBar readout, the labelled
 * GridWidget, and a New Game button. It also runs the game-setup prompt that
 * asks the player for a mine count between 10 and 20, and connects the grid's
 * status signals to the info bar.
 *
 * Inputs:  the player's mine-count choice; Qt resize events.
 * Outputs: the assembled window; startNewGame() calls into GridWidget.
 *
 * External sources: mine-count prompt and signal wiring added with assistance
 * from Claude (Anthropic), a generative AI assistant, 2026-09-17.
 */
#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QLabel>

class GridWidget;
class InfoBar;
class QPushButton;

enum class AiMode { Off, Interactive, Automatic };
enum class AiDifficulty { Easy, Medium, Hard };

struct GameSettings {
    int mines;
    bool new_mode;
    int start_time;
    AiMode aiMode;
    AiDifficulty aiDifficulty;
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

    // Modal dialog for the new-game options (mines, mode, AI).
    // Returns the player's choices. If they cancel, defaults are returned so
    // a game always starts.
    GameSettings askGameSettings();

    GridWidget *m_gridWidget = nullptr;
    InfoBar *m_infoBar = nullptr;
    QPushButton *m_newGameButton = nullptr;
};

#endif
