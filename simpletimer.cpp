#include "simpletimer.h"

SimpleTimer::SimpleTimer(QObject *parent)
    : QObject{parent}
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &SimpleTimer::timerFinished);
}

void SimpleTimer::timerFinished()
{
    qDebug()<<"timer finished";
}
