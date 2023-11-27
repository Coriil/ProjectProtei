#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTimer>
#include "Callprocessing.h"
#include "Configjson.h"
#include <queue>
#include <QMutex>


class ServerWorker : public QObject
{
    Q_OBJECT

public:
    QTimer* workerTimer;
    explicit ServerWorker(ConfigJson cfg);
    std::queue<int> queue;
    const size_t queueMaxSize;
    const size_t opNumber;
    std::vector<CallProcessing> operators;

    QMutex m_mtx;

    void checkQueue(long number); //У СHeck cqueue  и operatorsAssign общие данные - надо добавить мьютекс+

public slots:
     void operatorsAssign();
     void startWorker();

};

#endif // SERVERWORKER_H

