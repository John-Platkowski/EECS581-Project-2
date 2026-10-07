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