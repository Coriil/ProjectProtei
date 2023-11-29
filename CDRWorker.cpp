#include "CDRWorker.h"


CDRWorker::CDRWorker()
{

}

size_t CDRWorker::getRecordIndex(long ID)
{

    auto it = std::find_if(journal.begin(), journal.end(), [&ID](const record& r) {
        return r.callID == ID;
    }); //ЛЯМБДА-ФУНКЦИЯ СТРАШНО
    if (it != journal.end()) {
       return std::distance(journal.begin(),it);
    } else {
        return -1;
    }

}
void CDRWorker::startCDR()
{
    boost::log::add_common_attributes();

    boost::log::keywords::auto_flush = true;
    // Настройка логирования в файл "log.txt"
    boost::log::add_file_log(
     "log.txt",
    boost::log::keywords::format = "[%Message%]"
    );
    boost::log::keywords::auto_flush = true;
}

int CDRWorker::writeToFile(size_t id)
{
    QString newRecord;
    newRecord = journal[id].startCallDT.toString()+" "+journal[id].callID;
    BOOST_LOG_TRIVIAL(info) << " ";//newRecord.toStdString();
    return 0;
}
/*QDateTime inCallDT;//дата/время поступления вызова
long callID;//идентификатор вызова
long phoneNumber;//номер телефона
QDateTime finCallDT;//дата/время окончания вызова
int status;//статус (принят, перегрузка, превышено время ожидания)
QDateTime answDT;//дата/время ответа опретаора
int operID;//номер оператора
int callDuration;//продолжытельность звонка*/

void CDRWorker::recInCall(QDateTime inCall, long ID, long phNumber)
{
    record newRecord;
    newRecord.startCallDT = inCall;
    newRecord.callID = ID;
    newRecord.phoneNumber = phNumber;
    journal.push_back(newRecord);
    BOOST_LOG_TRIVIAL(info) << "вызов записан";
    //writeToFile(ID);

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
    writeToFile(id);
}




