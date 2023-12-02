#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTimer>
#include <algorithm>
#include "Callprocessing.h"
#include "Configjson.h"
#include "CDRWorker.h"
#include <QMutex>


class ServerWorker : public QObject
{
    Q_OBJECT
 struct caller{
    long  m_callerNumber;
    QTimer* m_callerTimer;
};
public:

    explicit ServerWorker(ConfigJson cfg);
    ~ServerWorker();
    QTimer* workerTimer;   
    std::vector<caller> callsQueue;//очередь, в которую попадают поступившие вызовы
    const size_t queueMaxSize;//а нужен ли вообще конст если будет перечтение?
    const size_t opNumber;
    const int busyOpTimeMin;
    const int busyOpTimeMax;
    int waitTimeMin;
    int waitTimeMax;
    std::vector<CallProcessing> operators;//операторы,эмуляция обработки вызовов c помощью таймера

    QMutex m_mtx;//мьютекс для защиты общих данных (очереди вызовов) во время операций
    //1. добавления в очередь 2. удаления из очереди при передаче опреатору 3. удаления из очереди из-за таймаута

    int checkQueue(long number);
signals:
    void answerCall(QDateTime ansDT, int opNum, long ID);//ответ оператора на вызов

public slots:
    void maintainQueue();
    void startWorker();
    void recFinishAnsweredCall(QDateTime finishDT, long ID);//окончание ответа опреатора - данные



};

#endif // SERVERWORKER_H

