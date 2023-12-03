#include <CDRWorker.h>

using namespace std;

CDRWorker::CDRWorker()
{

}

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


void CDRWorker::startCDR()
{
    boost::log::add_common_attributes();
    boost::log::add_file_log(// CDR
     "CDR.txt",
    boost::log::keywords::format = "[%Message%]", boost::log::keywords::auto_flush = true);
    BOOST_LOG_TRIVIAL(info) << "CDR started";

}

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

int CDRWorker::recFinishAnsweredCall(QDateTime finishDT, long number, long ID)
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
    writeToFile(ID);
    return 0;
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
    m_mtxCDR.unlock();
    writeToFile(ID);
}

void CDRWorker::recTimeoutedCalls(long timeoutedNumber)
{
    /*m_mtxCDR.lock();
    size_t ind = getRecordIndexByNumber(timeoutedNumber);
    journal[ind].status = TIMEOUT;
    m_mtxCDR.unlock();
    writeToFile(ind);*/

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
    writeToFile(ID);
}




