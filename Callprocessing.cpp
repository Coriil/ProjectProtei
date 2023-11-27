#include "Callprocessing.h"

CallProcessing::CallProcessing(ConfigJson cfg)
{
    m_BusyOpTimeMin = cfg.getOpTimeMin();
    m_BusyOpTimeMax = cfg.getOpTimeMax();
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &CallProcessing::releaseOp);//, Qt::DirectConnection);
    connect(this, &CallProcessing::computeData, this, &CallProcessing::assignOp);
}

CallProcessing::~CallProcessing()
{
    //delete m_timer;
}



void CallProcessing::assignOp(long number)//эмуляция занятости оператора
{
    qDebug() << number;
    srand((unsigned) time(NULL));
    //int OpBusyTimeMin = ConfigJson::getOpTimeMin();
    int randTime = m_BusyOpTimeMin + (rand() % (m_BusyOpTimeMax-m_BusyOpTimeMin));//случайное время занятости оператора
    //m_timer->setInterval(randTime*1000)
    m_timer->setInterval(5000);
    m_timer->start();
    qDebug()<<"timer N"<<m_timerNumber<<"started for" << randTime;

}

void CallProcessing::releaseOp()
{
    qDebug()<<"Operator N"<<m_timerNumber<<"finished";
    m_isBusy= false;//оператор освобождается

}

