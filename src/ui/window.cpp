// File: window.cpp
// Project: EECS 581 - Project 1: Minesweeper
// Description: Implementation of Window (QMainWindow subclass), assembling the central widget layout containing InfoBar and GridWidget, setting window geometry, and handling aspect-ratio grid resizing.
// Author: Jaydee Brown, Will Godderz
// Creation Date: 2026-09-17

/**
 * @file window.cpp
 * @brief Implementation of the top-level application window.
 * @author Jaydee Brown (original window), Will Godderz (game setup + documentation), Trey Timko (configured the game setup to allow for further customization, 2026-10-09)
 * @date 2026-09-17
 *
 * Builds the window layout, prompts for the mine count, ai, and mode settings at startup and on each
 * New Game, and connects GridWidget's status signals to the InfoBar.
 *
 * Inputs:  the player's mine-count choice, AI configuration, and gamemode configuration; Qt resize events.
 * Outputs: the rendered window; new-game commands to GridWidget.
 *
 * External sources: mine-count prompt and signal wiring added with assistance
 * from Claude (Anthropic), a generative AI assistant, 2026-09-17.
 */
#include "window.h"
#include "gridwidget.h"
#include "infobar.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QDialogButtonBox>

Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumSize(400, 400);
    resize(600,700);

    // create ui
    setupUi();

    // Trey Timko 10/09/2026
    // Ask for the game settings, then pass those settings
    // so the first board can be dealt.
    const GameSettings s = askGameSettings();
    m_gridWidget->startNewGame(s.mines, s.new_mode, s.start_time, s.aiMode, s.aiDifficulty);
}

Window::~Window() = default;

void Window::setupUi()
{
    // main widget for window
    auto *central = new QWidget(this);
    setCentralWidget(central);

    // create grid and scoreboard
    m_gridWidget = new GridWidget(central);
    m_infoBar = new InfoBar(central);

    m_newGameButton = new QPushButton("New Game", central);
    m_newGameButton->setFixedHeight(40);

    // Grid reports state changes; the info bar simply displays them.
    connect(m_gridWidget, &GridWidget::flagsRemainingChanged,
            m_infoBar, &InfoBar::setMineCount);
    connect(m_gridWidget, &GridWidget::statusChanged,
            m_infoBar, &InfoBar::setStatus);
    connect(m_newGameButton, &QPushButton::clicked,
            this, &Window::promptNewGame);
    //Connect the the info bar to the game timer
    connect(m_gridWidget, &GridWidget::timeChanged,
            m_infoBar, &InfoBar::setTime);

    // group infobar, grid and button together
    auto *gameLayout = new QVBoxLayout();
    gameLayout->setSpacing(5);
    gameLayout->setContentsMargins(0, 0, 0, 0);
    gameLayout->addWidget(m_infoBar);
    gameLayout->addWidget(m_gridWidget);
    gameLayout->addWidget(m_newGameButton);

    // nest group in outer layout
    auto *outerLayout = new QVBoxLayout(central);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setAlignment(Qt::AlignCenter);
    outerLayout->addLayout(gameLayout);

    setWindowTitle("Minesweeper");
}

// Trey Timko 10/09/2026
// Modal dialog for every new-game option in one window. 
//Nickan Safi 10/10/26
//Override other settings if using new mode
GameSettings Window::askGameSettings()
{
    QDialog dlg(this);
    dlg.setWindowTitle("New Game");
    QFormLayout *form = new QFormLayout(&dlg);

    // The box to enter mine count, a spin box
    // to allow for the numbers to be properly selected
    QSpinBox *mines = new QSpinBox(&dlg);
    mines->setRange(10, 20);
    mines->setValue(10);
    form->addRow("Number of mines:", mines);

    // A box for entering the mode, a combo box
    // so multiple preset options can be selected
    QComboBox *mode = new QComboBox(&dlg);
    mode->addItems({"Classic", "Timed"});
    form->addRow("Game mode:", mode);

    // A spin box again for easy digit selection,
    // this is for the time limit
    QSpinBox *time = new QSpinBox(&dlg);
    time->setRange(10, 600);
    time->setValue(120);
    form->addRow("Time limit in seconds:", time);

    // a combo box with preset options for selecting
    // the ai's mode
    QComboBox *aiMode = new QComboBox(&dlg);
    aiMode->addItems({"Off","Interactive","Automatic"});
    form->addRow("AI mode:", aiMode);

    // a combo box for selecting the ai's difficulty
    QComboBox *aiDiff = new QComboBox(&dlg);
    aiDiff->addItems({"Easy", "Medium", "Hard"});
    form->addRow("AI difficulty:", aiDiff);

    // the accept button
    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok, &dlg);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    dlg.exec();

    // The given settings are then stored into the corresponding variables.
    GameSettings s;                     
    s.mines = mines->value();
    s.new_mode = (mode->currentIndex() == 1);
    s.start_time = time->value();
    s.aiMode = static_cast<AiMode>(aiMode->currentIndex());
    s.aiDifficulty = static_cast<AiDifficulty>(aiDiff->currentIndex());
    if (s.new_mode) {
        s.mines = 10;
        s.aiMode = AiMode::Off;
    }
    return s;
}

void Window::promptNewGame()
{
    // Trey Timko 10/09/2026
    // prompts for a new game then passes those settings so the board can be dealt
    const GameSettings s = askGameSettings();
    m_gridWidget->startNewGame(s.mines, s.new_mode, s.start_time, s.aiMode, s.aiDifficulty);
}

// Added with assistance from ChatGPT (OpenAI), 2026-10-01.
// Uses the current grid width as the reference for scaling the InfoBar text.
void Window::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

}
