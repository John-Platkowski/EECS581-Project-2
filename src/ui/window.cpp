// File: window.cpp
// Project: EECS 581 - Project 1: Minesweeper
// Description: Implementation of Window (QMainWindow subclass), assembling the central widget layout containing InfoBar and GridWidget, setting window geometry, and handling aspect-ratio grid resizing.
// Author: Jaydee Brown, Will Godderz
// Creation Date: 2026-09-17

/**
 * @file window.cpp
 * @brief Implementation of the top-level application window.
 * @author Jaydee Brown (original window), Will Godderz (game setup + documentation)
 * @date 2026-09-17
 *
 * Builds the window layout, prompts for the mine count at startup and on each
 * New Game, and connects GridWidget's status signals to the InfoBar.
 *
 * Inputs:  player mine-count selection; Qt events.
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

    // Ask for the game settings and deal the first board.
    const GameSettings s = askGameSettings();
    m_gridWidget->startNewGame(s.mines, s.new_mode, s.start_time);
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

// Modal dialog for every new-game option in one window. 
GameSettings Window::askGameSettings()
{
    QDialog dlg(this);
    dlg.setWindowTitle("New Game");
    QFormLayout *form = new QFormLayout(&dlg);

    QSpinBox *mines = new QSpinBox(&dlg);
    mines->setRange(10, 20);
    mines->setValue(10);
    form->addRow("Number of mines:", mines);

    QComboBox *mode = new QComboBox(&dlg);
    mode->addItems({"Classic", "Timed"});
    form->addRow("Game mode:", mode);

    QSpinBox *time = new QSpinBox(&dlg);
    time->setRange(10, 600);
    time->setValue(120);
    form->addRow("Time limit in seconds:", time);

    QComboBox *aiMode = new QComboBox(&dlg);
    aiMode->addItems({"Off","Interactive","Automatic"});
    form->addRow("AI mode:", aiMode);

    QComboBox *aiDiff = new QComboBox(&dlg);
    aiDiff->addItems({"Easy", "Medium", "Hard"});
    form->addRow("AI difficulty:", aiDiff);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok, &dlg);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    dlg.exec();

    // On cancel return the default
    GameSettings s;                     
    s.mines = mines->value();
    s.new_mode = (mode->currentIndex() == 1);
    s.start_time = time->value();
    s.aiMode = static_cast<AiMode>(aiMode->currentIndex());
    s.aiDifficulty = static_cast<AiDifficulty>(aiDiff->currentIndex());
    return s;
}

void Window::promptNewGame()
{
    const GameSettings s = askGameSettings();
    m_gridWidget->startNewGame(s.mines, s.new_mode, s.start_time);
}

// Added with assistance from ChatGPT (OpenAI), 2026-10-01.
// Uses the current grid width as the reference for scaling the InfoBar text.
void Window::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

}
