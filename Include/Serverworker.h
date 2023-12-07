#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include <algorithm>
#include <Callprocessing.h>
#include <Configjson.h>
#include <CDRWorker.h>
#include <Caller.h>
#include <mylogger.h>

//перечисление возможных статусов при поступлении заявки
enum class WorkerStatus {
    OK = 0,
    DEFAULT = -1,
    OVERLOAD = -2,
    DUPLICATION = -3
};


//класс, обслуживающий очередь заявок
class ServerWorker : public QObject
{
    Q_OBJECT

private:
     QTimer* workerTimer = nullptr;//таймер для управления очередью
     const size_t queueMaxSize;//максимальный размер очереди
     const size_t opNumber;//количество опреаторов
     const int busyOpTimeMinSec;//нижний предел времени занятости опреатора
     const int busyOpTimeMaxSec;//верхний предел времени занятости опреатора
     const int waitTimeMinSec;//нижний предел времени ожидания заявки
     const int waitTimeMaxSec;//верхний предел времени ожидания заявки

     std::vector<Caller> callsQueue;//очередь, в которую попадают поступившие вызовы
     std::vector<CallProcessing> operators;//операторы,эмуляция обработки вызовов c помощью таймера
     QMutex mtx;//мьютекс для защиты общих данных (очереди вызовов) во время операций
                //1. добавления в очередь 2. удаления из очереди при передаче опреатору 3. удаления из очереди из-за таймаута

public:
    explicit ServerWorker(ConfigJson cfg);
    ~ServerWorker();
    WorkerStatus checkQueue(Caller &curentCaller);

signals:
    void answerCall(QDateTime ansDT, int opNum, long ID);//ответ оператора на вызов
    void finAnswerCall(QDateTime finishDT, long ID);
    void timeoutedCall(long ID);

public slots:
    void maintainQueue();
    void startWorker();
};

#endif // SERVERWORKER_H

