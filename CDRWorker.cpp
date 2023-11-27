#include "CDRWorker.h"

CDRWorker::CDRWorker(QObject *parent)
    : QObject{parent}
{

}

int CDRWorker::writeToFile()
{

}
QDateTime inCallDT;//дата/время поступления вызова
long callID;//идентификатор вызова
long phoneNumber;//номер телефона
QDateTime finCallDT;//дата/время окончания вызова
int status;//статус (принят, перегрузка, превышено время ожидания)
QDateTime answDT;//дата/время ответа опретаора
int operID;//номер оператора
int callDuration;//продолжытельность звонка

void CDRWorker::recInCall(QDateTime inCall, long ID, long phNumber)
{
    record newRecord;
    newRecord.startCallDT = inCall;
    newRecord.callID = ID;
    newRecord.phoneNumber = phNumber;
    journal.push_back(newRecord);

}

void CDRWorker::recAnswerCall(QDateTime ansDT, int opNum, long ID)
{
     size_t id = getRecordIndex(ID);
     journal[id].answDT = ansDT;
     journal[id].operNum = opNum;
}

void CDRWorker::recFinishAnsweredCall(QDateTime finishDT, long ID)
{
    size_t id = getRecordIndex(ID);
    journal[id].finCallDT = finishDT;
    journal[id].callDuration = journal[id].finCallDT.secsTo(journal[id].startCallDT);
    journal[id].status = 0;

}
