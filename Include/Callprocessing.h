#ifndef CALLPROCESSING_H
#define CALLPROCESSING_H
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QObject>
#include <QThread>
#include "Configjson.h"
#include "CDRWorker.h"

class CallProcessing : public QObject
{
    Q_OBJECT
private:
    QTimer* m_timer;
    int m_timerNumber = 0;
    int m_BusyOpTimeMin;
    int m_BusyOpTimeMax;
    long m_processedNumber;
    long m_processedID;

public:
    bool m_isBusy;
    CallProcessing();
    ~CallProcessing();
    void setNumber(int i){m_timerNumber=i;};
    void setBusyOpTimeMin(int a){m_BusyOpTimeMin=a;};
    void setBusyOpTimeMax(int b){m_BusyOpTimeMax=b;};

signals:
    void computeData(long number, long ID);
    void finishAnsweredCall(QDateTime finishDT, long number, long ID);//окончание ответа опреатора

private slots:
    void assignOp(long number, long ID);
    void releaseOp();
};

#endif // CALLPROCESSING_H
