#include "operators.h"

Operators::Operators()
{

    busynessTimer.setSingleShot(true);
    busynessTimer.setInterval(5000);
    isBusy = false;
    //connect(this, &Operators::startCall, this, &Operators::notAvailable);
    //connect(this, &Operators::finishCall, this, &Operators::availableNow);
}

int Operators::assignOp()//эмуляция занятости оператора
{
    //emit startCall(currentOp.number);
    isBusy = true;//оператор занят
    srand((unsigned) time(NULL));
    int randTime = 10 + (rand() % 11);//случайное время занятости оператора
    busynessTimer.start(randTime*100);
    qDebug()<<"timer N"<<number<<"started";
    return 0;
}

int Operators::releaseOp(Operators currentOp)
{
    currentOp.isBusy= false;//оператор освобождается
    qDebug()<<"timer N"<<number<<"finished";
    //emit finishCall(currentOp.number);
    return 0;
    //  сигнал для добавления оператора в стек свободных операторов.
}

int Operators::initAvailableOp(int opNum)
{
    /*
    for (int i = 0; i < opNum; i++)//сравнение
    {
        allOp[i].isBusy=false;
        allOp[i].number=i;

    }*/
    /*availableOp.resize(opNum);
    for (int i :  availableOp)
    {
        if (!allOp[i].isBusy)
            availableOp[i]=allOp[i].number;
    }*/

    return 0;
}

/*void Operators::availableNow()
{
    int i = find(availableOp.begin(),availableOp.end(),number);

}

void Operators::notAvailable()
{

}*/

