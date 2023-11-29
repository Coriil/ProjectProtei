#ifndef CDRWORKER_H
#define CDRWORKER_H

#include <QObject>
#include <QFile>
#include <QDateTime>
#include <QTimer>
#include <QFile>
#include <QThread>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

class CDRWorker : public QObject
{
    Q_OBJECT
    struct record
    {
        QDateTime startCallDT;//дата/время поступления вызова
        long callID;//идентификатор вызова
        long phoneNumber;//номер телефона
        QDateTime finCallDT;//дата/время окончания вызова
        int status;//статус (принят, перегрузка, превышено время ожидания)
        QDateTime answDT;//дата/время ответа опретаора
        int operNum;//номер оператора
        int callDuration;//продолжытельность звонка

    };
    enum callStatus//набор статусов вызова
    {
        CALL_OK = 0,
        TIMEOUT = -1,
        OVERLOAD = -2,
        CALL_DUPLICATION=-3,//может это не здесь, хз
        NO_STATUS = -4
    };
    std::vector<record> journal;

public:
    explicit CDRWorker();
    size_t getRecordIndex(long ID);



signals:
    /*void inCall(QDateTime inCall, long ID, long phNumber);//входящий вызов
    void answerCall(QDateTime ansDT, int opNum, long ID);//ответ оператора на вызов
    void finishAnsweredCall(QDateTime finishDT, long ID);//окончание ответа опреатора*/
    void callOverload();//вызов не принят (перегрузка)
    void callTimeout();//превышено время ожидания

public slots:
    void startCDR();
    int writeToFile(size_t id);
    //bool clearJournal();

    void recInCall(QDateTime inCall, long ID, long phNumber);//входящий вызов - данные
    void recAnswerCall(QDateTime ansDT, int opNum, long ID);//ответ оператора на вызов - данные
    void recFinishAnsweredCall(QDateTime finishDT, long ID);//окончание ответа опреатора - данные
    //void recCallOverload();//вызов не принят(перегрузка) - данные
    //void recCallTimeout();//превышено время ожидания - данные

};

#endif // CDRWORKER_H

