#include "CDRWorker.h"

using namespace std;
CDRWorker::CDRWorker()
{

}

size_t CDRWorker::getRecordIndex(long ID)
{
    auto it = std::find_if(journal.begin(), journal.end(), [&ID](const record& r) {
        return r.callID == ID;});
    if (it != journal.end()) {
       return std::distance(journal.begin(),it);
    } else {
        return -1;
    }

}
size_t CDRWorker::getRecordIndexByNumber(long number)
{
    auto it = std::find_if(journal.begin(), journal.end(), [&number](const record& r) {
        return r.phoneNumber == number;});
    if (it != journal.end()) {
       return std::distance(journal.begin(),it);
    } else {
        return -1;
    }

}

void CDRWorker::startCDR()
{
    boost::log::add_common_attributes();
    boost::log::add_file_log(// CDR
     "CDR.txt",
    boost::log::keywords::format = "[%Message%]", boost::log::keywords::auto_flush = true);
    BOOST_LOG_TRIVIAL(info) << "CDR started";

}

int CDRWorker::writeToFile(size_t ind)
{
    m_mtxCDR.lock();
    record currentRec = journal[ind];
    journal.erase(journal.begin()+ind);
    QDateTime curDT=currentRec.startCallDT;
    QString curDTstr =  curDT.toString("dd.MM.yyyy hh:mm:ss");
    string startDT = curDTstr.toStdString();
    string finishDT;
    string opNum;
    string answerDT;
    string callDuration;
    if (currentRec.callDuration==-1)
    {
        callDuration = "None";
    }
    else
    {
       callDuration = to_string(currentRec.callDuration);
    }
    if (currentRec.operNum == -1)
    {
        opNum = "None";
    }
    else
    {
        opNum =  to_string(currentRec.operNum);
    }

    switch (currentRec.status)
    {
        case CALL_OK:
            finishDT = (currentRec.finCallDT.toString("dd.MM.yyyy hh:mm:ss")).toStdString();
            answerDT = (currentRec.answDT.toString("dd.MM.yyyy hh:mm:ss")).toStdString();
            break;
        case TIMEOUT:
            finishDT = " ";
            answerDT = " ";
            break;
        case OVERLOAD:
            finishDT = startDT;
            answerDT = " ";
            break;
        case CALL_DUPLICATION:
            finishDT = startDT;
            answerDT = " ";
            break;
        default:
            finishDT = " ";
            answerDT = (currentRec.answDT.toString("dd.MM.yyyy hh:mm:ss")).toStdString();
            break;
    }
    string status = callStatusToString(currentRec.status);
    string newRecord =
            "call started:" + startDT +
            " callID:"+ to_string(currentRec.callID) +
            " ph.Number:" + to_string(currentRec.phoneNumber)+
            " call finished:" + finishDT +
            " status:" + status +
            " operator №:" + opNum +
            " duration:" + callDuration;
    BOOST_LOG_TRIVIAL(info) << newRecord;
    m_mtxCDR.unlock();
    return 0;
}
/*QDateTime inCallDT;//дата/время поступления вызова
long callID;//идентификатор вызова
long phoneNumber;//номер телефона
QDateTime finCallDT;//дата/время окончания вызова
int status;//статус (принят, перегрузка, превышено время ожидания)
QDateTime answDT;//дата/время ответа опретаора
int operID;//номер оператора
int callDuration;//продолжительность звонка*/

void CDRWorker::recInCall(QDateTime inCall, long ID, long phNumber)
{
    //m_mtxCDR.lock();
    record newRecord;
    newRecord.startCallDT = inCall;
    newRecord.callID = ID;
    newRecord.phoneNumber = phNumber;
    newRecord.status = NOT_FINISHED;
    journal.push_back(newRecord);
    //m_mtxCDR.unlock();
    //size_t id = getRecordIndex(ID);
    //writeToFile(id);
}

void CDRWorker::recAnswerCall(QDateTime ansDT, int opNum, long number)
{
    //m_mtxCDR.lock();
    if (getRecordIndexByNumber(number)!=-1)
    {
    size_t id = getRecordIndexByNumber(number);
    journal[id].answDT = ansDT;
    journal[id].operNum = opNum;
   // m_mtxCDR.unlock();
     //writeToFile(id);
    }
}

void CDRWorker::recFinishAnsweredCall(QDateTime finishDT, long number)
{
    m_mtxCDR.lock();
    size_t id = getRecordIndexByNumber(number);
    journal[id].finCallDT = finishDT;
    journal[id].callDuration = journal[id].answDT.secsTo(journal[id].finCallDT);
    journal[id].status = CALL_OK;
    m_mtxCDR.unlock();
    writeToFile(id);
}

void CDRWorker::recCallOverload(QDateTime inCall,long ID, long phNumber)
{
    m_mtxCDR.lock();
    record newRecord;
    newRecord.startCallDT = inCall;
    newRecord.callID = ID;
    newRecord.phoneNumber = phNumber;
    newRecord.status = OVERLOAD;
    journal.push_back(newRecord);
    size_t id = getRecordIndex(ID);
    m_mtxCDR.unlock();
    writeToFile(id);
}

void CDRWorker::recTimeoutedCalls(long timeoutedNumber)
{
    m_mtxCDR.lock();
    size_t ind = getRecordIndexByNumber(timeoutedNumber);
    journal[ind].status = TIMEOUT;
    m_mtxCDR.unlock();
    writeToFile(ind);

}

void CDRWorker::recCallDuplication(QDateTime inCall, long ID, long phNumber)
{
    m_mtxCDR.lock();
    record newRecord;
    newRecord.startCallDT = inCall;
    newRecord.callID = ID;
    newRecord.phoneNumber = phNumber;
    newRecord.status = CALL_DUPLICATION;
    journal.push_back(newRecord);
    m_mtxCDR.unlock();
    size_t id = getRecordIndex(ID);
    writeToFile(id);
}




