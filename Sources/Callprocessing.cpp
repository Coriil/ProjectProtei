#include <Callprocessing.h>

CallProcessing::CallProcessing()
{
    timerOperator = new QTimer(this);
    timerOperator->setSingleShot(true);
    connect(timerOperator, &QTimer::timeout, this, &CallProcessing::releaseOp);//
    connect(this, &CallProcessing::computeData, this, &CallProcessing::assignOp);
}

CallProcessing::~CallProcessing()
{
    delete timerOperator;
}

//эмуляция занятости оператора
void CallProcessing::assignOp(long number, long ID)
{
    m_processedNumber = number;
    m_processedID = ID;
    srand(time(NULL));
    int randTime = busyOpTimeMinSec + (rand() % (busyOpTimeMaxSec-busyOpTimeMinSec));//случайное время занятости оператора, диапазон настраивается в конфигурации
    timerOperator->setInterval(randTime*1000);
    timerOperator->start();
    qDebug()<<"timer N"<<operatorNumber<<"started for" << randTime;
}

//освобождение опреатора после обработки заявки
void CallProcessing::releaseOp()
{
    qDebug()<<"Operator N"<<operatorNumber<<"finished";
    m_isBusy= false;//оператор освобождается
    QDateTime curDT = QDateTime::currentDateTime();
    emit finishAnsweredCall(curDT, m_processedID);
}

