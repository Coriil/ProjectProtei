#include <CDRWorker.h>

using namespace std;

CDRWorker::CDRWorker()
{

}

//преобразует статус вызова в строку
string CDRWorker::callStatusToString(callStatus code)
{
    switch (code)
    {
        case callStatus::CALL_OK: return "Ok       ";
        case callStatus::TIMEOUT: return "Timeout  ";
        case callStatus::OVERLOAD: return "Overload";
        case callStatus::CALL_DUPLICATION: return "Call duplication";
        case callStatus::NOT_FINISHED: return "Not defined";
        default: return "Unknown error code";
    }
}

//получает индекс записи во внутреннем журнале по ID (Идентификатору вызова)
int CDRWorker::getRecordIndex(long ID)
{
    auto it = std::find_if(journal.begin(), journal.end(), [&ID](const record& r) {
        return r.callID == ID;});
    if (it != journal.end()) {
        return std::distance(journal.begin(),it);
    } else {
        return -1;
    }
}

//инициализирует файл CDR для записи данных о вызовах
void CDRWorker::startCDR()
{
    boost::log::add_common_attributes();
    boost::log::add_file_log(// CDR
     "CDR.txt",
    boost::log::keywords::format = "[%Message%]", boost::log::keywords::auto_flush = true);
    BOOST_LOG_TRIVIAL(info) << "CDR started";

}

//добавляет запись в файл CDR
int CDRWorker::writeToFile(long ID)
{
    m_mtxCDR.lock();
    int ind = getRecordIndex(ID);
    if (ind < 0)
    {
        m_mtxCDR.unlock();
        return -1;
    }
    record currentRec = journal[ind];
    journal.erase(journal.begin()+ind);
    QDateTime curDT=currentRec.startCallDT;
    QString curDTstr =  curDT.toString("dd.MM.yyyy hh:mm:ss");
    string startDT = curDTstr.toStdString();
    string finishDT;
    string answerDT;
    string opNum;
    string callDuration;

    if (currentRec.callDuration==-1)
        callDuration = "None";
    else
       callDuration = to_string(currentRec.callDuration);

    if (currentRec.operNum == -1)
        opNum = "None";
    else
        opNum =  to_string(currentRec.operNum);

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
            "; callID:"+ to_string(currentRec.callID) +
            "; phone number:" + to_string(currentRec.phoneNumber)+
            "; call finished:" + finishDT +
            "; status:" + status +
            "; operator №:" + opNum +
            "; duration:" + callDuration;
    BOOST_LOG_TRIVIAL(info) << newRecord;
    m_mtxCDR.unlock();
    return 0;
}

//добавляет во внутренний журнал запись о входящем вызове
void CDRWorker::recInCall(QDateTime inCall, long ID, long phNumber)
{
    m_mtxCDR.lock();
    record newRecord;
    newRecord.startCallDT = inCall;
    newRecord.callID = ID;
    newRecord.phoneNumber = phNumber;
    newRecord.status = NOT_FINISHED;
    journal.push_back(newRecord);
    m_mtxCDR.unlock();
}

//Добавляет во внутренний журнал запись об ответе оператора на вызов
int CDRWorker::recAnswerCall(QDateTime ansDT, int opNum, long ID)
{
    m_mtxCDR.lock();
    int ind = getRecordIndex(ID);
    if (ind < 0)
    {
        m_mtxCDR.unlock();
        return -1;
    }
    journal[ind].answDT = ansDT;
    journal[ind].operNum = opNum;
    m_mtxCDR.unlock();
    return 0;
}

//Добавляет во внутренний журнал запись об окончании ответа опреатора на вызов
//и отправляет запись на вывод в CDR
int CDRWorker::recFinishAnsweredCall(QDateTime finishDT, long ID)
{
    m_mtxCDR.lock();
    int ind = getRecordIndex(ID);
    if (ind < 0)
    {
        m_mtxCDR.unlock();
        return -1;
    }
    journal[ind].finCallDT = finishDT;
    journal[ind].callDuration = journal[ind].answDT.secsTo(journal[ind].finCallDT);
    journal[ind].status = CALL_OK;
    m_mtxCDR.unlock();
    return writeToFile(ID);;
}

//Добавляет во внутренний журнал запись о перегрузке (поступление нового вызова при полной очереди)
//и отправляет запись на вывод в CDR
int CDRWorker::recCallOverload(QDateTime inCall,long ID, long phNumber)
{
    m_mtxCDR.lock();
    record newRecord;
    newRecord.startCallDT = inCall;
    newRecord.callID = ID;
    newRecord.phoneNumber = phNumber;
    newRecord.status = OVERLOAD;
    journal.push_back(newRecord);
    m_mtxCDR.unlock();
    return writeToFile(ID);
}


int CDRWorker::recTimeoutedCalls(long ID)
{
    m_mtxCDR.lock();
    size_t ind = getRecordIndex(ID);
    journal[ind].status = TIMEOUT;
    m_mtxCDR.unlock();
    return writeToFile(ID);
}

//Добавляет во внутренний журнал запись о дублировании вызова (вызов с таким номером уже есть в очереди)
//и отправляет запись на вывод в CDR
int CDRWorker::recCallDuplication(QDateTime inCall, long ID, long phNumber)
{
    m_mtxCDR.lock();
    record newRecord;
    newRecord.startCallDT = inCall;
    newRecord.callID = ID;
    newRecord.phoneNumber = phNumber;
    newRecord.status = CALL_DUPLICATION;
    journal.push_back(newRecord);
    m_mtxCDR.unlock();
    return writeToFile(ID);
}




