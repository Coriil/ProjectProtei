#ifndef OPERATORS_H
#define OPERATORS_H
#include <QTimer>
#include <QTime>
#include <QDebug>

class Operators //: public QObject
{
      //Q_OBJECT
private:

    int number;
    QTimer busynessTimer;
    bool isBusy;

    //std::vector<Operators> allOp;
    //std::vector<int> availableOp;

public:
    Operators();
    bool isOpBusy(Operators currentOp){return currentOp.isBusy;}
    void setNumber(int i){number=i;};
    int assignOp();
    int releaseOp(Operators currentOp);
    int initAvailableOp(int opNumber);
/*public slots:
    void availableNow();
    void notAvailable();
signals:
    void startCall(int opNum);
    void finishCall(int opNum);*/

};




#endif // OPERATORS_H
