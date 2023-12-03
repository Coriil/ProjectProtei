#ifndef CDRWORKER_H
#define CDRWORKER_H

#include <QObject>
#include <QFile>
#include <QDateTime>
#include <QTimer>
#include <QFile>
#include <QThread>
#include <QMutex>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

//класс для ведения call detailed record и вывода записей в файл CDR.txt
class CDRWorker : public QObject
{
    Q_OBJECT

private:
    //набор статусов вызова
    enum callStatus
    {
        CALL_OK = 0,
        TIMEOUT = -1,
        OVERLOAD = -2,
        CALL_DUPLICATION=-3,//может это не здесь, хз
        NOT_FINISHED = -4
    };
    //структура одиночной записи в журнале
    struct record
    {
        QDateTime startCallDT;//дата/время поступления вызова
        long callID = 0;//идентификатор вызова
        long phoneNumber = 0;//номер телефона
        QDateTime finCallDT;//дата/время окончания вызова
        callStatus status;//статус (принят, перегрузка, превышено время ожидания)
        QDateTime answDT;//дата/время ответа опретаора
        int operNum = -1;//номер оператора
        int callDuration = -1;//продолжительность звонка
    };
    std::vector<record> journal;
    std::string callStatusToString(callStatus code);
    QMutex m_mtxCDR;
    int getRecordIndex(long ID);
    int writeToFile(long ID);

public:

    CDRWorker();

signals:
    void callTimeout();//превышено время ожидания

public slots:
    void startCDR();
    void recInCall(QDateTime inCall, long ID, long phNumber);//входящий вызов
    int recAnswerCall(QDateTime ansDT, int opNum, long ID);//ответ оператора на вызов
    int recFinishAnsweredCall(QDateTime finishDT, long ID);//окончание ответа опреатора
    int recCallOverload(QDateTime inCall,long ID, long phNumber);//вызов не принят(перегрузка)
    void recTimeoutedCalls(long timeoutedNumber);//превышено время ожидания - данные
    int recCallDuplication(QDateTime inCall, long ID, long phNumber);//дублирование вызова
};

#endif // CDRWORKER_H

