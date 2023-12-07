#include <Serverworker.h>

ServerWorker::ServerWorker(ConfigJson cfg) : queueMaxSize(cfg.getQueueSize()), opNumber(cfg.getOpNumber()),
    busyOpTimeMinSec(cfg.getOpTimeMin()), busyOpTimeMaxSec(cfg.getOpTimeMax()),
    waitTimeMinSec(cfg.getWaitTimeMin()),waitTimeMaxSec(cfg.getWaitTimeMax())
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
    for (size_t i = 0; i < operators.size(); ++i)//инициализация операторов, которые будут обрабатывать вызовы
    {
        operators[i].setNumber(i);
        operators[i].setBusyOpTimeMin(busyOpTimeMinSec);
        operators[i].setBusyOpTimeMax(busyOpTimeMaxSec);
        connect(&operators[i], &CallProcessing::finishAnsweredCall, this,&ServerWorker::finAnswerCall);
    }
    workerTimer = new QTimer();
    connect(workerTimer, &QTimer::timeout, this, &ServerWorker::maintainQueue);
    workerTimer->start(1000);
}





void ServerWorker::maintainQueue()//назначение опрератора и удаление вызовов из очереди
{

        mtx.lock();
        //удаление заявок с истекшим временем ожидания

        std::vector <Caller> waitingCalls;
        for (size_t i = 0; i < callsQueue.size(); i++)
        {
            if (callsQueue[i].getTimeoutDT() < QDateTime::currentDateTime())
            {
                emit  timeoutedCall(callsQueue[i].callerID);
                BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::info) << "Call with ID: " << std::to_string(callsQueue[i].callerID) <<
                                                                             "has timeouted";

            }
            else
                waitingCalls.push_back(callsQueue[i]);
        }
        callsQueue = waitingCalls;
        //распределение заявки из очереди на свободного оператора
        for (size_t i = 0; i < operators.size(); i++)
        {
            if (operators[i].isBusy == false && callsQueue.empty() == false)
            {
                long number = callsQueue.front().callerNumber;
                long ID = callsQueue.front().callerID;
                operators[i].isBusy = true;
                operators[i].processCall(number,ID);//назначение операторов
                QDateTime curDT = QDateTime::currentDateTime();
                emit answerCall(curDT, i, ID);
                BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::info) << "Call with ID " << std::to_string(ID) <<
                                                                             " sent to the operator № " << std::to_string(i);
                callsQueue.erase(callsQueue.begin());
                BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::info) << "Queue size is " << std::to_string(callsQueue.size());
            }

        }
        mtx.unlock();
}


//проверяет очередь, проверяет случаи перегрузки и дублирования вызовов, добавляет вызов в очередь
WorkerStatus ServerWorker::checkQueue(Caller &currentCaller)//
{
    WorkerStatus status = WorkerStatus::DEFAULT;
    mtx.lock();
    if (callsQueue.size() >= queueMaxSize)
    {
        status = WorkerStatus::OVERLOAD;//overload - очередь заполнена
    }
    else
    {
        long number = currentCaller.callerNumber;
        auto it = find_if(callsQueue.begin(), callsQueue.end(), [&number] ( const Caller& c) {return c.callerNumber == number;});
        if (it==callsQueue.end())
        {
            srand(time(NULL));
            qint64 randTime = waitTimeMinSec + (rand() % (waitTimeMaxSec-waitTimeMinSec));
            int setTimeout = currentCaller.setTimeoutDTSecs(randTime);
            if (setTimeout == 0)
            {
                callsQueue.push_back(currentCaller);
                status=WorkerStatus::OK;//номер добавляется в очередь
                BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::info) << "Number " << std::to_string(number) <<
                                                                             " added to queue";
            }
            else //не удалось установить время ожидания
                status=WorkerStatus::DEFAULT;
        }
        else
        {
            status=WorkerStatus::DUPLICATION;//дубликация вызова - номер уже в очереди

        }

    }
    mtx.unlock();
    return status;
}
