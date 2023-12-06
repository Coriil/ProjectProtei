#ifndef CALLER_H
#define CALLER_H

#include <QDateTime>

//заявка - номер телефона и идентификатор опреатора
class Caller
{
private:
    QDateTime inputDT; //время поступления заявки
    QDateTime timeoutDT; // время окончания ожидания заявки (timeout)

public:
    Caller(long callerNum, long ID);
    long callerNumber;//номер телефона заявки
    long callerID;//идентификатор телефона заявки

    int setTimeoutDTSecs(qint64 timeoutTimeSec);
    QDateTime getTimeoutDT() {return timeoutDT;}
};

#endif // CALLER_H
