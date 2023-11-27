#ifndef CALLPROCESSING_H
#define CALLPROCESSING_H
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QObject>
#include <iostream>
#include <QThread>

class CallProcessing : public QObject
{
    QTimer* m_timer;
    int m_timerNumber = 0;
     Q_OBJECT

public:
     bool m_isBusy;
    //std::vector<CallProcessing> allOp;
    CallProcessing();
    ~CallProcessing();

    void setNumber(int i){m_timerNumber=i;};
    int initAvailableOp(int opNumber);

signals:
    void computeData(int number);

private slots:
    void assignOp(int number);
    void releaseOp();

};

#endif // CALLPROCESSING_H
