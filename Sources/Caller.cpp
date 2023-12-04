#include "Caller.h"



Caller::Caller(long callerNum, long ID)
{
    callerNumber = callerNum;
    callerID = ID;
    inputDT = QDateTime::currentDateTime();
}

void Caller::setTimeoutDT(qint64 timeoutTimeSec)
{
    timeoutDT = inputDT.addSecs(timeoutTimeSec);
}
