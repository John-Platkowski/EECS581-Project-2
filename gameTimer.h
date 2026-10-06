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

    signals:
    void timeChanged(int seconds);

    private slots:
        void updateTime();

    private:
        QTimer timer;
        int elapsedTime;
};

#endif