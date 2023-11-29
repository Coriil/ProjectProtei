#ifndef CALLPROCESSING_H
#define CALLPROCESSING_H
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QObject>
#include <iostream>
#include <QThread>
#include "Configjson.h"
#include "CDRWorker.h"

class CallProcessing : public QObject
{
    QTimer* m_timer;
    int m_timerNumber = 0;
    int m_BusyOpTimeMin;
    int m_BusyOpTimeMax;
    Q_OBJECT

public:
     bool m_isBusy;

     explicit CallProcessing();
     ~CallProcessing();

    void setNumber(int i){m_timerNumber=i;};
    void setBusyOpTimeMin(int a){m_BusyOpTimeMin=a;};
    void setBusyOpTimeMax(int b){m_BusyOpTimeMax=b;};

    int initAvailableOp(int opNumber);

signals:
    void computeData(long number);

    void finishAnsweredCall(QDateTime finishDT, long ID);//окончание ответа опреатора

private slots:
    void assignOp(long number);
    void releaseOp();

};

#endif // CALLPROCESSING_H
