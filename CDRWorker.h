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

class CDRWorker : public QObject
{
    Q_OBJECT

private:
    enum callStatus//набор статусов вызова
    {
        CALL_OK = 0,
        TIMEOUT = -1,
        OVERLOAD = -2,
        CALL_DUPLICATION=-3,//может это не здесь, хз
        NOT_FINISHED = -4
    };
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

public:
    explicit CDRWorker();
    size_t getRecordIndex(long ID);
    size_t getRecordIndexByNumber(long number);

signals:
    void callTimeout();//превышено время ожидания

public slots:
    void startCDR();
    int writeToFile(size_t id);
    void recInCall(QDateTime inCall, long ID, long phNumber);//входящий вызов - данные
    void recAnswerCall(QDateTime ansDT, int opNum, long phNumber);//ответ оператора на вызов - данные
    void recFinishAnsweredCall(QDateTime finishDT, long number, long ID);//окончание ответа опреатора - данные
    void recCallOverload(QDateTime inCall,long ID, long phNumber);//вызов не принят(перегрузка) - данные
    void recTimeoutedCalls(long timeoutedNumber);//превышено время ожидания - данные
    void recCallDuplication(QDateTime inCall, long ID, long phNumber);
};

#endif // CDRWORKER_H

