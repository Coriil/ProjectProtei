#include "Serverworker.h"

ServerWorker::ServerWorker(int opNumber, int queueSize) : queueMaxSize(queueSize), opNumber(opNumber)
{

}


void ServerWorker::startWorker() {
    operators = std::vector<CallProcessing>(opNumber);
    for (size_t i = 0; i < operators.size(); ++i)
    {
        operators[i].setNumber(i);
    }

    workerTimer = new QTimer();
    connect(workerTimer, &QTimer::timeout, this, &ServerWorker::operatorsAssign);
    workerTimer->start(1000); // Вызываем функцию каждsые N мс
}

void ServerWorker::operatorsAssign() {
         for (size_t i = 0; i < operators.size(); ++i)
        {
            if (operators[i].m_isBusy == false && queue.empty() == false)
            {
                qDebug() <<"Operators assign";
                int number = queue.front();
                operators[i].m_isBusy = true;
                operators[i].computeData(number);
                qDebug() << "Queue size"<<queue.size();
                queue.pop();
            }
        }
}

void ServerWorker::checkQueue(int number) {
    qDebug() <<"Check queue";
    if (queue.size() >= queueMaxSize)
    {
        qDebug() <<"Queue is full";
    }
    else
        queue.push(number);
}
