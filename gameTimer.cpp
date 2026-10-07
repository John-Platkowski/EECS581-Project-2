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
    addTime(15);
}

//Adds 10 seconds when the user flags a mine correctly
void GameTimer::correctFlag() {
    addTime(10);
}

//Removes 5 seconds when the user incorrectly flags a square 
    //Prevents user from just clicking randomly to see if the timer increases and knowing where all the mines are
void GameTimer::incorrectFlag() {
    addTime(-5);
}

//Setter for countdown vars 
void GameTimer::setCountdown(bool timed, int sec){
    countdown = timed;
    startTime = sec;
}