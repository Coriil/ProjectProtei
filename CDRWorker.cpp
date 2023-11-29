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
size_t CDRWorker::getRecordIndexByNumber(long number)
{

    auto it = std::find_if(journal.begin(), journal.end(), [&number](const record& r) {
        return r.phoneNumber == number;
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

   // Настройка логирования в файл "log.txt"
    boost::log::add_file_log(
     "log.txt",
    boost::log::keywords::format = "[%Message%]", boost::log::keywords::auto_flush = true);

 //b
}

int CDRWorker::writeToFile(size_t id)
{

    std::string newRecord;
    QDateTime curDT=journal[id].startCallDT;
    QString curDTstr =  curDT.toString("dd.MM.yyyy hh:mm:ss");
    //newRecord = journal[id].startCallDT.toString()+" "+journal[id].phoneNumber;
    newRecord = curDTstr.toStdString() +" "+std::to_string(journal[id].callID) + " " +
            std::to_string(journal[id].phoneNumber) + " " + (journal[id].finCallDT.toString()).toStdString() +
            " " + std::to_string(journal[id].status) + " " + std::to_string(journal[id].operNum) + " " + std::to_string(journal[id].callDuration);
    //for (int i =0; i<400;i++)
    BOOST_LOG_TRIVIAL(info) << newRecord;// << " " << i;
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
    record newRecord;
    newRecord.startCallDT = inCall;
    newRecord.callID = ID;
    newRecord.phoneNumber = phNumber;
    newRecord.status = -4;
    journal.push_back(newRecord);

}

void CDRWorker::recAnswerCall(QDateTime ansDT, int opNum, long number)//ек
{
     size_t id = getRecordIndexByNumber(number);
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




