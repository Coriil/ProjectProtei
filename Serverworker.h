#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTimer>
#include <algorithm>
#include "Callprocessing.h"
#include "Configjson.h"
#include "CDRWorker.h"
#include <QMutex>

enum class WorkerStatus {
    OK = 0,
    DEFAULT = -1,
    OVERLOAD = -2,
    DUPLICATE = -3
};

class ServerWorker : public QObject
{
    Q_OBJECT
 struct caller
 {
    long  m_callerNumber;
    long m_callerID;
};
private:
     QTimer* workerTimer;
     const size_t queueMaxSize;//а нужен ли вообще конст если будет перечтение?
     const size_t opNumber;
     const int busyOpTimeMin;
     const int busyOpTimeMax;
     int waitTimeMin;
     int waitTimeMax;
     std::vector<caller> callsQueue;//очередь, в которую попадают поступившие вызовы
     std::vector<CallProcessing> operators;//операторы,эмуляция обработки вызовов c помощью таймера
     QMutex m_mtx;//мьютекс для защиты общих данных (очереди вызовов) во время операций
     //1. добавления в очередь 2. удаления из очереди при передаче опреатору 3. удаления из очереди из-за таймаута

public:
    explicit ServerWorker(ConfigJson cfg);
    ~ServerWorker();
    WorkerStatus checkQueue(long number, long id);

signals:
    void answerCall(QDateTime ansDT, int opNum, long ID);//ответ оператора на вызов
    void finAnswerCall(QDateTime finishDT, long number, long ID);
    void timeoutedCalls(long timeoutedNumber);

public slots:
    void maintainQueue();
    void startWorker();
    void getFinishAnsweredCall(QDateTime finishDT,long number, long ID);
};

#endif // SERVERWORKER_H

