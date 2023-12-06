#ifndef CALLPROCESSING_H
#define CALLPROCESSING_H
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QObject>

//класс для эмуляции обработки вызова оператором
class CallProcessing : public QObject
{
    Q_OBJECT
private:
    QTimer* timerOperator; //таймер для эмуляции занятости оператора
    int operatorNumber = 0; //номер оператора (начиная с 0)
    int busyOpTimeMinSec;//нижний предел времени занятости опреатора
    int busyOpTimeMaxSec;//верхний предел времени занятости опреатора
    long processedNumber;//номер телефона, который обслуживает оператор
    long processedID;//идентификатор телефона, который обслуживает оператор

public:
    CallProcessing();
    ~CallProcessing();
    bool isBusy;//статус оператора (занят/свободен)
    void setNumber(int i){operatorNumber=i;};
    void setBusyOpTimeMin(int a){busyOpTimeMinSec=a;};
    void setBusyOpTimeMax(int b){busyOpTimeMaxSec=b;};

signals:
    void processCall(long number, long ID);
    void finishAnsweredCall(QDateTime finishDT, long ID);//окончание ответа опреатора

private slots:
    void assignOp(long number, long ID);
    void releaseOp();
};

#endif // CALLPROCESSING_H
