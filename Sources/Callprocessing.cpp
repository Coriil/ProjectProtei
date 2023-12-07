#include <Callprocessing.h>

CallProcessing::CallProcessing()
{
    timerOperator = new QTimer(this);
    timerOperator->setSingleShot(true);
    connect(timerOperator, &QTimer::timeout, this, &CallProcessing::releaseOp);//
    connect(this, &CallProcessing::processCall, this, &CallProcessing::assignOp);
}

CallProcessing::~CallProcessing()
{
    delete timerOperator;
}

//эмуляция занятости оператора
void CallProcessing::assignOp(long number, long ID)
{
    processedNumber = number;
    processedID = ID;
    srand(time(NULL));
    int randTime = busyOpTimeMinSec + (rand() % (busyOpTimeMaxSec-busyOpTimeMinSec));//случайное время занятости оператора, диапазон настраивается в конфигурации
    timerOperator->setInterval(randTime*1000);
    timerOperator->start();
    BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::info)<< "Operator №" << std::to_string(operatorNumber) <<
                                                                " answering, for " <<std::to_string(randTime) << " sec";
}

//освобождение опреатора после обработки заявки
void CallProcessing::releaseOp()
{
    isBusy= false;//оператор освобождается
    QDateTime curDT = QDateTime::currentDateTime();
    emit finishAnsweredCall(curDT, processedID);
    BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::info)<< "Operator №" << std::to_string(operatorNumber) <<
                                                                " finished call";
}

