#include "Caller.h"



Caller::Caller(long callerNum, long ID)
{
    callerNumber = callerNum;
    callerID = ID;
    inputDT = QDateTime::currentDateTime();
}

//функция устанавливает время ожидания в секундах
int Caller::setTimeoutDTSecs(qint64 timeoutTimeSec)
{
    if (!inputDT.isNull())
    {
        timeoutDT = inputDT.addSecs(timeoutTimeSec);
        return 0;
    }
    else
        return -1;
}
