#ifndef CDRWORKER_H
#define CDRWORKER_H

#include <QObject>

class CDRWorker : public QObject
{
    Q_OBJECT
public:
    explicit CDRWorker(QObject *parent = nullptr);

signals:

};

#endif // CDRWORKER_H
