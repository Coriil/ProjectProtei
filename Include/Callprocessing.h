#ifndef CALLPROCESSING_H
#define CALLPROCESSING_H
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QObject>

class CallProcessing : public QObject
{
    Q_OBJECT
private:
    QTimer* timerOperator;
    int operatorNumber = 0;
    int busyOpTimeMinSec;
    int busyOpTimeMaxSec;
    long m_processedNumber;
    long m_processedID;

public:
    CallProcessing();
    ~CallProcessing();
    bool m_isBusy;
    void setNumber(int i){operatorNumber=i;};
    void setBusyOpTimeMin(int a){busyOpTimeMinSec=a;};
    void setBusyOpTimeMax(int b){busyOpTimeMaxSec=b;};

signals:
    void computeData(long number, long ID);
    void finishAnsweredCall(QDateTime finishDT, long ID);//окончание ответа опреатора
    void timeoutCall(long ID);

private slots:
    void assignOp(long number, long ID);
    void releaseOp();
};

#endif // CALLPROCESSING_H
