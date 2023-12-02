#include "Serverworker.h"

ServerWorker::ServerWorker(ConfigJson cfg) : queueMaxSize(cfg.getQueueSize()), opNumber(cfg.getOpNumber()),
    busyOpTimeMin(cfg.getOpTimeMin()), busyOpTimeMax(cfg.getOpTimeMax()),
    waitTimeMin(cfg.getWaitTimeMin()),waitTimeMax(cfg.getWaitTimeMax())
{


}

ServerWorker::~ServerWorker()
{
    delete workerTimer;
}


void ServerWorker::startWorker()
{
    operators = std::vector<CallProcessing>(opNumber);
    for (size_t i = 0; i < operators.size(); ++i)
    {
        operators[i].setNumber(i);
        operators[i].setBusyOpTimeMin(busyOpTimeMin);
        operators[i].setBusyOpTimeMax(busyOpTimeMax);
    }
    workerTimer = new QTimer();
    connect(workerTimer, &QTimer::timeout, this, &ServerWorker::maintainQueue);

    workerTimer->start(500);
}




void ServerWorker::maintainQueue()//назначение опрератора и удаление вызовов из очереди
{
        m_mtx.lock();
        auto timeouted = std::remove_if(callsQueue.begin(),callsQueue.end(),[]( caller& c)
        {return (!c.m_callerTimer->isActive());});
        callsQueue.erase(timeouted,callsQueue.end());//удаление заявок с истекшим временем ожидания

        for (size_t i = 0; i < operators.size(); i++)
        {
            if (operators[i].m_isBusy == false && callsQueue.empty() == false)
            {
                qDebug() <<"Operators assign";
                long number = callsQueue.front().m_callerNumber;
                operators[i].m_isBusy = true;
                operators[i].computeData(number);//назначение операторов
                QDateTime curDT = QDateTime::currentDateTime();
                emit answerCall(curDT, i, number);
                qDebug() << "Queue size"<<callsQueue.size();
                callsQueue.erase(callsQueue.begin());
            }

        }
        m_mtx.unlock();
}


int ServerWorker::checkQueue(long number)//
{
    int isFull=0;
    qDebug() <<"Check queue";
    m_mtx.lock();
    if (callsQueue.size() >= queueMaxSize)
    {
        qDebug() <<"Queue is full";
        isFull=-1;//overload - очередь заполнена

    }
    else
    {
        auto it = find_if(callsQueue.begin(), callsQueue.end(), [&number] ( const caller& c) {return c.m_callerNumber ==number;});
        if (it==callsQueue.end())
        {
            srand(time(NULL));
            int randTime = waitTimeMin + (rand() % (waitTimeMax-waitTimeMin));
            caller newCaller;
            newCaller.m_callerNumber = number;
            newCaller.m_callerTimer = new QTimer();
            newCaller.m_callerTimer->setSingleShot(true);
            newCaller.m_callerTimer->setInterval(randTime*1000);
            callsQueue.push_back(newCaller);
            callsQueue.back().m_callerTimer->start();
            isFull=0;//номер добавляется в очередь
        }
        else
            isFull=-2;//дубликация вызова - номер уже в очереди

    }
    m_mtx.unlock();
    return isFull;
}
