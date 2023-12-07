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
        case callStatus::CALL_OK: return "Call answered (ok) ";
        case callStatus::TIMEOUT: return "Timeout ";
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
 BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::info) << "CDR journal";
}

//добавляет запись в файл CDR
int CDRWorker::writeRecord(long ID)
{
    mtxCDR.lock();
    int ind = getRecordIndex(ID);
    if (ind < 0)
    {
        mtxCDR.unlock();
        BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::warning) << "could not find record with ID:" <<
                                                                        std::to_string(ID)<<", possible data loss";
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
    //запись о вызове в зависимости от его статуса
    switch (currentRec.status)
    {
        case CALL_OK:
            finishDT = (currentRec.finCallDT.toString("dd.MM.yyyy hh:mm:ss")).toStdString();
            answerDT = (currentRec.answDT.toString("dd.MM.yyyy hh:mm:ss")).toStdString();
            break;
        case TIMEOUT:
            finishDT = (currentRec.finCallDT.toString("dd.MM.yyyy hh:mm:ss")).toStdString();
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
            answerDT = " ";
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
    writeToFile(newRecord);
    mtxCDR.unlock();
    BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::info) << "record added to CDR.txt";
    return 0;
}

//запись в файл данных о вызове
int CDRWorker::writeToFile(std::string record) {
    QFile file("CDR.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << QString::fromStdString(record) << "\n";
        file.close();
        return 0;
    } else {
        BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::warning) << "file open error";
    }
    return -1;
}

//добавляет во внутренний журнал запись о входящем вызове
void CDRWorker::recInCall(QDateTime inCall, long ID, long phNumber)
{
    mtxCDR.lock();
    record newRecord;
    newRecord.startCallDT = inCall;
    newRecord.callID = ID;
    newRecord.phoneNumber = phNumber;
    newRecord.status = NOT_FINISHED;
    journal.push_back(newRecord);
    mtxCDR.unlock();
    BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::info) << "new call: " << std::to_string(phNumber) <<
                                                                 ", ID: " << std::to_string(ID);
}

//Добавляет во внутренний журнал запись об ответе оператора на вызов
int CDRWorker::recAnswerCall(QDateTime ansDT, int opNum, long ID)
{
    mtxCDR.lock();
    int ind = getRecordIndex(ID);
    if (ind < 0)
    {
        mtxCDR.unlock();
        BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::warning) << "could not find record with ID:" <<
                                                                        std::to_string(ID)<<", possible data loss";
        return -1;
    }
    journal[ind].answDT = ansDT;
    journal[ind].operNum = opNum;
    mtxCDR.unlock();
    return 0;
}

//Добавляет во внутренний журнал запись об окончании ответа опреатора на вызов
//и отправляет запись на вывод в CDR
int CDRWorker::recFinishAnsweredCall(QDateTime finishDT, long ID)
{
    mtxCDR.lock();
    int ind = getRecordIndex(ID);
    if (ind < 0)
    {
        mtxCDR.unlock();
        BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::warning) << "could not find record with ID:" <<
                                                                        std::to_string(ID)<<", possible data loss";
        return -1;
    }
    journal[ind].finCallDT = finishDT;
    journal[ind].callDuration = journal[ind].answDT.secsTo(journal[ind].finCallDT);
    journal[ind].status = CALL_OK;
    mtxCDR.unlock();
    return writeRecord(ID);
}

//Добавляет во внутренний журнал запись о перегрузке (поступление нового вызова при полной очереди)
//и отправляет запись на вывод в CDR
int CDRWorker::recCallOverload(QDateTime inCall,long ID, long phNumber)
{
    mtxCDR.lock();
    record newRecord;
    newRecord.startCallDT = inCall;
    newRecord.callID = ID;
    newRecord.phoneNumber = phNumber;
    newRecord.status = OVERLOAD;
    journal.push_back(newRecord);
    mtxCDR.unlock();
    return writeRecord(ID);
}

//Добавляет во внутренний журнал запись об окончании времени ожидания заявки
//и отправляет запись на вывод в CDR
int CDRWorker::recTimeoutedCall(long ID)
{
    mtxCDR.lock();
    size_t ind = getRecordIndex(ID);
    if (ind < 0)
    {
        mtxCDR.unlock();
        BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::warning) << "could not find record with ID:" <<
                                                                        std::to_string(ID)<<", possible data loss";
        return -1;

    }
    journal[ind].status = TIMEOUT;
    journal[ind].finCallDT = QDateTime::currentDateTime();
    mtxCDR.unlock();
    return writeRecord(ID);
}

//Добавляет во внутренний журнал запись о дублировании вызова (вызов с таким номером уже есть в очереди)
//и отправляет запись на вывод в CDR
int CDRWorker::recCallDuplication(QDateTime inCall, long ID, long phNumber)
{
    mtxCDR.lock();
    record newRecord;
    newRecord.startCallDT = inCall;
    newRecord.callID = ID;
    newRecord.phoneNumber = phNumber;
    newRecord.status = CALL_DUPLICATION;
    journal.push_back(newRecord);
    mtxCDR.unlock();
    return writeRecord(ID);
}




