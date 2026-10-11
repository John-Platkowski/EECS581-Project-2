/*
#Prologue Comment
#Module: gameTimer.cpp
#Description: Implements the GameTimer class, which manages the Minesweeper game's timer. It supports counting up or down, adjusting time based on player actions, and notifying other components when the time changes.
#Input:    setCountdown requires timed: whether countdown mode is enabled, and sec: the starting countdown time in seconds.
#           addTime requires seconds: the amount of time to add or remove.
#           Player actions call revealSquare, correctFlag, or incorrectFlag to adjust the timer accordingly.
#Output:   Updates and provides the current timer value in seconds.
#           Emits timeChanged whenever the timer value is updated.
#           Prevents the timer from dropping below zero.
#           Revealing a square adds 15 seconds, correctly flagging a mine adds 10 seconds, and incorrectly flagging a square removes 5 seconds.
#Authors:
#   Tyler Oswald
#   Lena Palmieri
#Creation Date:
#   October 6, 2026
*/

#include "gameTimer.h"

GameTimer::GameTimer(QObject *parent) : QObject(parent), elapsedTime(0) {
    connect(&timer, &QTimer::timeout, this, &GameTimer::updateTime);
}

void GameTimer::start() {
    timer.start(1000);
}

void GameTimer::stop() {
    timer.stop();
}

void GameTimer::reset() {
    timer.stop();
    //If the countdown flag is set, we reset to the countdown start time
    //Otherwise, we count up from 0
    elapsedTime = countdown ? startTime : 0;

    emit timeChanged(elapsedTime);
}

//Adds/removes time from the timer
void GameTimer::addTime(int seconds) {
    elapsedTime += seconds;

    emit timeChanged(elapsedTime);
}

int GameTimer::getTime(){
    return elapsedTime;
}

//Updates the timer for each second
void GameTimer::updateTime() {
    //Count down 
    if(countdown){
        elapsedTime += -1;
    }
    //Count up
    else{
        elapsedTime += 1;
    }

    //Timer cannot go below 0
    if (elapsedTime < 0) {
        elapsedTime = 0; 
    }
    emit timeChanged(elapsedTime);
}

//Adds 15 seconds when the player reveals a square
void GameTimer::revealSquare() {
    if (countdown)
        addTime(15);
}

//Adds 10 seconds when the user flags a mine correctly
void GameTimer::correctFlag() {
    if (countdown)
        addTime(10);
}

//Removes 5 seconds when the user incorrectly flags a square 
    //Prevents user from just clicking randomly to see if the timer increases and knowing where all the mines are
void GameTimer::incorrectFlag() {
    if (countdown)
        addTime(-5);
}

void GameTimer::correctUnflag() {
    if (countdown) {
        addTime(-10);
    }
}

//Setter for countdown vars 
void GameTimer::setCountdown(bool timed, int sec){
    countdown = timed;
    startTime = sec;
}