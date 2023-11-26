#include "Callprocessing.h"

CallProcessing::CallProcessing()
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &CallProcessing::releaseOp);//, Qt::DirectConnection);
    connect(this, &CallProcessing::computeData, this, &CallProcessing::assignOp);
}

CallProcessing::~CallProcessing()
{
    //delete m_timer;
}



void CallProcessing::assignOp(int number)//эмуляция занятости оператора
{
    qDebug() << number;


    srand((unsigned) time(NULL));
    int randTime = 10 + (rand() % 11);//случайное время занятости оператора
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

