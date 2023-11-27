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
    explicit ServerWorker(ConfigJson cfg);
    ~ServerWorker();
    QTimer* workerTimer;
    std::queue<int> queue;
    const size_t queueMaxSize;//а нужен ли вообще конст если будет перечтение?
    const size_t opNumber;
    const int BusyOpTimeMin;
    const int BusyOpTimeMax;
    std::vector<CallProcessing> operators;

    QMutex m_mtx;

    bool checkQueue(long number);

public slots:
     void operatorsAssign();
     void startWorker();

};

#endif // SERVERWORKER_H

