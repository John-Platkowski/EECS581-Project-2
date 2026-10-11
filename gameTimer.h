/*
#Prologue Comment
#Module: GameTimer.h
#Description: Declares the GameTimer class, which manages the Minesweeper game's timer. It supports elapsed-time tracking and countdown functionality, allows time to be added or reset, and provides methods to track square reveals and correct or incorrect flags.
#Input: Initialization optionally requires a parent QObject. The countdown timer can be configured with timed: whether countdown mode is enabled, and sec: the starting time in seconds. Additional time can be specified through addTime(seconds).
#Output:    timeChanged emits the updated time in seconds.
#           Provides access to the current time through getTime().
#           Supports starting, stopping, and resetting the timer.
#           Supports countdown mode and time adjustments.
#Authors:
#   Lena Palmieri
#   Tyler Oswald
#Creation Date:
#   October 6, 2026
*/

#ifndef GAMETIMER_H
#define GAMETIMER_H

#include <QObject>
#include <QTimer>

class GameTimer : public QObject {
    Q_OBJECT

    public:
        explicit GameTimer(QObject *parent = nullptr);

        void start();
        void stop();
        void reset();

        void addTime(int seconds);
        int getTime();

        void revealSquare();
        void correctFlag();
        void incorrectFlag();
        void correctUnflag();

        //Timer that counts down
        void setCountdown(bool timed, int sec);

    signals:
    void timeChanged(int seconds);

    private slots:
        void updateTime();

    private:
        QTimer timer;
        int elapsedTime;
        //Flag to control if timer counts down or not 
        bool countdown = false;
        //This sets the amount of time the countdown timer will start with
        int  startTime = 0;  
};

#endif