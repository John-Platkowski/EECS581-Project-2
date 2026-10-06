include "GameTimer.h"

GameTimer::GameTimer(QObject *parent) : QObject(parent), elapsedTime(0) {
    connect(&timer, &QTimer::timeout, this, &GameTimer::updateTime);
}

GameTimer::start() {
    timer.start(300);
}

GameTimer::stop() {
    timer.stop();
}

GameTimer::reset() {
    timer.stop();
    elapsedTime = 0;

    emit timeChanged(elapsedTime);
}

//Adds/removes time from the timer
GameTimer::addTime(int seconds) {
    elapsedTime += seconds;

    emit timeChanged(elapsedTime);
}

GameTimer::getTime(){
    return elapsedTime;
}

//Updates the timer for each second
GameTimer::updateTime() {
    elapsedTime++;

    emit timeChanged(elapsedTime);
}

//Adds 15 seconds when the player reveals a square
GameTimer::revealSquare() {
    addTime(15);
}

//Adds 10 seconds when the user flags a mine correctly
GameTimer::correctFlag() {
    addTime(10);
}

//Removes 5 seconds when the user incorrectly flags a square 
    //Prevents user from just clicking randomly to see if the timer increases and knowing where all the mines are
GameTimer::incorrectFlag() {
    addTime(-5);
}