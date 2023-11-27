#include "Serverworker.h"

ServerWorker::ServerWorker(ConfigJson cfg) : queueMaxSize(cfg.getQueueSize()), opNumber(cfg.getOpNumber()), BusyOpTimeMin(cfg.getOpTimeMin()), BusyOpTimeMax(cfg.getOpTimeMax())
{

}

ServerWorker::~ServerWorker()
{

}


void ServerWorker::startWorker() {
    operators = std::vector<CallProcessing>(opNumber);
    for (size_t i = 0; i < operators.size(); ++i)
    {
        operators[i].setNumber(i);
        operators[i].setBusyOpTimeMin(BusyOpTimeMin);
        operators[i].setBusyOpTimeMax(BusyOpTimeMax);
    }
    workerTimer = new QTimer();
    connect(workerTimer, &QTimer::timeout, this, &ServerWorker::operatorsAssign);
    workerTimer->start(1000); // Вызываем функцию каждsые N мс
}


void ServerWorker::operatorsAssign() {
        m_mtx.lock();
         for (size_t i = 0; i < operators.size(); ++i)
        {
            if (operators[i].m_isBusy == false && queue.empty() == false)
            {
                qDebug() <<"Operators assign";
                long number = queue.front();
                operators[i].m_isBusy = true;
                operators[i].computeData(number);
                qDebug() << "Queue size"<<queue.size();
                queue.pop();
            }

        }
        m_mtx.unlock();
}

void ServerWorker::checkQueue(long number) {
    qDebug() <<"Check queue";
    m_mtx.lock();
    if (queue.size() >= queueMaxSize)
    {
        qDebug() <<"Queue is full";
    }
    else
        queue.push(number);
    m_mtx.unlock();
}
