#include "CDRWorker.h"


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

   // Настройка логирования в файл "log.txt"
    boost::log::add_file_log(
     "log.txt",
    boost::log::keywords::format = "[%Message%]", boost::log::keywords::auto_flush = true);
    BOOST_LOG_TRIVIAL(info) << "log started";
 //b
}

int CDRWorker::writeToFile(size_t ind)
{
    using namespace std;
    string newRecord;
    QDateTime curDT=journal[ind].startCallDT;
    QString curDTstr =  curDT.toString("dd.MM.yyyy hh:mm:ss");
    //newRecord = journal[id].startCallDT.toString()+" "+journal[id].phoneNumber;
    string startDT = curDTstr.toStdString();
    string ID = to_string(journal[ind].callID);
    string phoneNum = to_string(journal[ind].phoneNumber);
    string status = callStatusToString(journal[ind].status);
    string finDT;
    string opNum;
    string answerDT;
    string callDuration;
    switch (journal[ind].status)
    {
        case CALL_OK:
            finDT = (journal[ind].finCallDT.toString("dd.MM.yyyy hh:mm:ss")).toStdString();
            answerDT = (journal[ind].answDT.toString("dd.MM.yyyy hh:mm:ss")).toStdString();
            opNum = to_string(journal[ind].operNum);
            callDuration = to_string(journal[ind].callDuration);
            break;
        case TIMEOUT:
            finDT = " ";//нет
            answerDT = " ";
            opNum = " ";
            callDuration = " ";
            break;
        case OVERLOAD:
            finDT = startDT;
            answerDT = " ";
            opNum = " ";
            callDuration = " ";
            break;
        case CALL_DUPLICATION:
            finDT = startDT;
            answerDT = " ";
            opNum = " ";
            callDuration = " ";
            break;
        default:
            finDT = " ";
            answerDT = (journal[ind].answDT.toString("dd.MM.yyyy hh:mm:ss")).toStdString();
            opNum = to_string(journal[ind].operNum);
            callDuration = " ";
            break;
    }

    newRecord = "call started:" + startDT +" callID:"+ ID + " ph.Number:" + phoneNum + " call finished:" + finDT +
            " status:" + status + " operator №:" + opNum + " duration:" + callDuration;
    BOOST_LOG_TRIVIAL(info) << newRecord;// << " " << i;
    journal.erase(journal.begin()+ind);

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
    size_t id = getRecordIndexByNumber(phNumber);
    writeToFile(id);
}




