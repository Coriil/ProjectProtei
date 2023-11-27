#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTimer>
#include "Callprocessing.h"
#include <queue>
#include <mutex>


class ServerWorker : public QObject
{
    Q_OBJECT

public:
    QTimer* workerTimer;
    ServerWorker(int opNumber, int queueSize);//Это говно, передавать весь конфиг нужно
    std::queue<int> queue;
    const size_t queueMaxSize;
    const size_t opNumber;
    std::vector<CallProcessing> operators;
    explicit ServerWorker(QObject *parent = nullptr);

    void checkQueue(int number); //У СHeck cqueue  и operatorsAssign общие данные - надо добавить мьютекс

public slots:
     void operatorsAssign();
     void startWorker();

};

#endif // SERVERWORKER_H

