#ifndef CALLER_H
#define CALLER_H

#include <QDateTime>

//заявка - номер телефона и идентификатор опреатора
class Caller
{

private:
    QDateTime inputDT;
    QDateTime timeoutDT;

public:
    long  callerNumber;
    long callerID;

    Caller(long callerNum, long ID);;
    void setTimeoutDTSecs(qint64 timeoutTimeSec);

    QDateTime getTimeoutDT() {return timeoutDT;}
};

#endif // CALLER_H
