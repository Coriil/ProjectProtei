#include <Callprocessing.h>

CallProcessing::CallProcessing()
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &CallProcessing::releaseOp);//
    connect(this, &CallProcessing::computeData, this, &CallProcessing::assignOp);
}

CallProcessing::~CallProcessing()
{
    delete m_timer;
}

void CallProcessing::assignOp(long number, long ID)//эмуляция занятости оператора
{
    m_processedNumber = number;
    m_processedID = ID;
    srand(time(NULL));
    int randTime = m_BusyOpTimeMin + (rand() % (m_BusyOpTimeMax-m_BusyOpTimeMin));//случайное время занятости оператора, диапазон настраивается в конфигурации
    //m_timer->setInterval(randTime*1000);
    m_timer->setInterval(10000);
    m_timer->start();
    qDebug()<<"timer N"<<m_timerNumber<<"started for" << randTime;
}

void CallProcessing::releaseOp()
{
    qDebug()<<"Operator N"<<m_timerNumber<<"finished";
    m_isBusy= false;//оператор освобождается
    QDateTime curDT = QDateTime::currentDateTime();
    emit finishAnsweredCall(curDT, m_processedNumber, m_processedID);
}
