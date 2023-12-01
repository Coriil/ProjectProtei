#include "Serverworker.h"

ServerWorker::ServerWorker(ConfigJson cfg) : queueMaxSize(cfg.getQueueSize()), opNumber(cfg.getOpNumber()), BusyOpTimeMin(cfg.getOpTimeMin()), BusyOpTimeMax(cfg.getOpTimeMax())
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
        operators[i].setBusyOpTimeMin(BusyOpTimeMin);
        operators[i].setBusyOpTimeMax(BusyOpTimeMax);
    }
    workerTimer = new QTimer();
    connect(workerTimer, &QTimer::timeout, this, &ServerWorker::operatorsAssign);
    workerTimer->start(500);
}


void ServerWorker::operatorsAssign()
{
        m_mtx.lock();
        for (size_t i = 0; i < operators.size(); ++i)
        {
            if (operators[i].m_isBusy == false && callsQueue.empty() == false)
            {
                qDebug() <<"Operators assign";
                long number = callsQueue.front();
                operators[i].m_isBusy = true;
                operators[i].computeData(number);
                QDateTime curDT = QDateTime::currentDateTime();
                emit answerCall(curDT, i, number);
                qDebug() << "Queue size"<<callsQueue.size();
                callsQueue.pop();
            }

        }
        m_mtx.unlock();
}

bool ServerWorker::checkQueue(long number)//
{
    bool isFull=false;
    qDebug() <<"Check queue";
    m_mtx.lock();
    if (callsQueue.size() >= queueMaxSize)
    {
        qDebug() <<"Queue is full";//ответ на уровне сервера нужен
        isFull=true;
        //overload
    }
    else
    {
        isFull=false;
        callsQueue.push(number);
    }
    m_mtx.unlock();
    return isFull;
}
