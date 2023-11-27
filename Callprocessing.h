#ifndef CALLPROCESSING_H
#define CALLPROCESSING_H
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QObject>
#include <iostream>
#include <QThread>
#include "Configjson.h"

class CallProcessing : public QObject
{
    QTimer* m_timer;
    int m_timerNumber = 0;
    int m_BusyOpTimeMin;
    int m_BusyOpTimeMax;
    Q_OBJECT

public:
     bool m_isBusy;
    //std::vector<CallProcessing> allOp;
    CallProcessing(ConfigJson cfg);
    ~CallProcessing();

    void setNumber(int i){m_timerNumber=i;};

    int initAvailableOp(int opNumber);

signals:
    void computeData(long number);

private slots:
    void assignOp(long number);
    void releaseOp();

};

#endif // CALLPROCESSING_H
