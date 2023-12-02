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
        connect(&operators[i], &CallProcessing::finishAnsweredCall, this,&ServerWorker::getFinishAnsweredCall);
    }
    workerTimer = new QTimer();
    connect(workerTimer, &QTimer::timeout, this, &ServerWorker::maintainQueue);
    workerTimer->start(1000);
}

void ServerWorker::getFinishAnsweredCall(QDateTime finishDT, long number, long ID)
{
    emit finAnswerCall(finishDT, number, ID);
}




void ServerWorker::maintainQueue()//назначение опрератора и удаление вызовов из очереди
{
       /* m_mtx.lock();
        if (0)//(callsQueue.empty() == false)
        {
        auto timeouted = std::remove_if(callsQueue.begin(),callsQueue.end(),[]( caller& c)
        {return (!c.m_callerTimer->isActive());});
        std::vector<caller> timeoutedCallers;
        if (timeouted!=callsQueue.end())
        {
            qDebug() << "timeout";
            timeoutedCallers.assign(timeouted,callsQueue.end());
            for (auto &caller :timeoutedCallers)
            emit timeoutedCalls(caller.m_callerNumber);
            callsQueue.erase(timeouted,callsQueue.end());//удаление заявок с истекшим временем ожидания
        }
        }
        m_mtx.unlock();*/
        m_mtx.lock();
        for (size_t i = 0; i < operators.size(); i++)
        {
            if (operators[i].m_isBusy == false && callsQueue.empty() == false)
            {
                qDebug() <<"Operators assign";
                long number = callsQueue.front().m_callerNumber;
                long ID = callsQueue.front().m_callerID;
                operators[i].m_isBusy = true;
                operators[i].computeData(number,ID);//назначение операторов
                QDateTime curDT = QDateTime::currentDateTime();
                emit answerCall(curDT, i, number);
                qDebug() << "Queue size"<<callsQueue.size();
                callsQueue.erase(callsQueue.begin());
            }

        }
        m_mtx.unlock();
}


int ServerWorker::checkQueue(long number, long ID)//
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
            newCaller.m_callerID = ID;
            callsQueue.push_back(newCaller);
            //callsQueue.back().m_callerNumber = number;
            //callsQueue.back().m_callerTimer->start(randTime*1000);
            isFull=0;//номер добавляется в очередь
        }
        else
        {
            isFull=-2;//дубликация вызова - номер уже в очереди
        }

    }
    m_mtx.unlock();
    return isFull;
}
