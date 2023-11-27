#ifndef CDRWORKER_H
#define CDRWORKER_H

#include <QObject>
#include <QFile>
#include <QDateTime>
#include <QTimer>
#include <QFile>

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
    QTimer* writeToFileTimer;

public:
    explicit CDRWorker(QObject *parent = nullptr);
    size_t getRecordIndex(long ID);


signals:
    void inCall(QDateTime inCall, long ID, long phNumber);//входящий вызов
    void answerCall();//ответ оператора на вызов
    void finishAnsweredCall();//окончание ответа опреатора
    void callOverload();//вызов не принят (перегрузка)
    void callTimeout();//превышено время ожидания

public slots:
    int writeToFile();
    bool clearJournal();

    void recInCall(QDateTime inCall, long ID, long phNumber);//входящий вызов - запись
    void recAnswerCall(QDateTime ansDT, int opNum, long ID);//ответ оператора на вызов - запись
    void recFinishAnsweredCall(QDateTime finishDT, long ID);//окончание ответа опреатора - запись
    void recCallOverload();//вызов не принят(перегрузка) - запись
    void recCallTimeout();//превышено время ожидания - запись

};

#endif // CDRWORKER_H

