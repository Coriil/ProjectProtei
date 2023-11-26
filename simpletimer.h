#ifndef SIMPLETIMER_H
#define SIMPLETIMER_H

#include <QObject>
#include <QTimer>
#include <QDebug>

class SimpleTimer : public QObject
{
    QTimer* m_timer;
    int m_timerNumber;
    Q_OBJECT
public:
    explicit SimpleTimer(QObject *parent = nullptr);

signals:
public slots:
    void timerFinished();

};

#endif // SIMPLETIMER_H
