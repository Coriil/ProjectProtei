#include <Serverworker.h>

ServerWorker::ServerWorker(ConfigJson cfg) : queueMaxSize(cfg.getQueueSize()), opNumber(cfg.getOpNumber()),
    busyOpTimeMin(cfg.getOpTimeMin()), busyOpTimeMax(cfg.getOpTimeMax()),
    waitTimeMin(cfg.getWaitTimeMin()),waitTimeMax(cfg.getWaitTimeMax())
{


}

ServerWorker::~ServerWorker()
{
    if (workerTimer!=nullptr)
        delete workerTimer;
}

//запуск обслуживания очереди
void ServerWorker::startWorker()
{
    operators = std::vector<CallProcessing>(opNumber);
    for (size_t i = 0; i < operators.size(); ++i)
    {
        operators[i].setNumber(i);
        operators[i].setBusyOpTimeMin(busyOpTimeMin);
        operators[i].setBusyOpTimeMax(busyOpTimeMax);
        connect(&operators[i], &CallProcessing::finishAnsweredCall, this,&ServerWorker::finAnswerCall);
    }
    workerTimer = new QTimer();
    connect(workerTimer, &QTimer::timeout, this, &ServerWorker::maintainQueue);
    workerTimer->start(1000);
}





void ServerWorker::maintainQueue()//назначение опрератора и удаление вызовов из очереди
{

        m_mtx.lock();
        //удаление заявок с истекшим временем ожидания

        std::vector <Caller> waitingCalls;
        for (size_t i = 0; i < callsQueue.size(); i++)
        {
            if (callsQueue[i].getTimeoutDT() < QDateTime::currentDateTime())
                emit  timeoutedCall(callsQueue[i].callerID);
            else
                waitingCalls.push_back(callsQueue[i]);
        }
        callsQueue = waitingCalls;
        //распределение заявки из очереди на свободного оператора
        for (size_t i = 0; i < operators.size(); i++)
        {
            if (operators[i].m_isBusy == false && callsQueue.empty() == false)
            {
                qDebug() <<"Operators assign";
                long number = callsQueue.front().callerNumber;
                long ID = callsQueue.front().callerID;
                operators[i].m_isBusy = true;
                operators[i].computeData(number,ID);//назначение операторов
                QDateTime curDT = QDateTime::currentDateTime();
                emit answerCall(curDT, i, ID);
                qDebug() << "Queue size"<<callsQueue.size();
                callsQueue.erase(callsQueue.begin());
            }

        }
        m_mtx.unlock();
}


//проверяет очередь, проверяет случаи перегрузки и дублирования вызовов, добавляет вызов в очередь
WorkerStatus ServerWorker::checkQueue(Caller &currentCaller)//
{
    WorkerStatus status = WorkerStatus::DEFAULT;
    qDebug() <<"Check queue";
    m_mtx.lock();
    if (callsQueue.size() >= queueMaxSize)
    {
        qDebug() <<"Queue is full";
        status = WorkerStatus::OVERLOAD;//overload - очередь заполнена

    }
    else
    {
        long number = currentCaller.callerNumber;
        auto it = find_if(callsQueue.begin(), callsQueue.end(), [&number] ( const Caller& c) {return c.callerNumber == number;});
        if (it==callsQueue.end())
        {
            srand(time(NULL));
            qint64 randTime = waitTimeMin + (rand() % (waitTimeMax-waitTimeMin));
            currentCaller.setTimeoutDTSecs(randTime);
            callsQueue.push_back(currentCaller);

            //callsQueue.back().m_callerNumber = number;
            //callsQueue.back().m_callerTimer->start(randTime*1000);
            status=WorkerStatus::OK;//номер добавляется в очередь
        }
        else
        {
            status=WorkerStatus::DUPLICATE;//дубликация вызова - номер уже в очереди
        }

    }
    m_mtx.unlock();
    return status;
}
