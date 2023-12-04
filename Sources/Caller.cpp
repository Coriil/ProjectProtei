#include "Caller.h"



Caller::Caller(long callerNum, long ID)
{
    callerNumber = callerNum;
    callerID = ID;
    inputDT = QDateTime::currentDateTime();
}

//функция устанавливает время ожидания в секундах
void Caller::setTimeoutDTSecs(qint64 timeoutTimeSec)
{
    timeoutDT = inputDT.addSecs(timeoutTimeSec);
}
