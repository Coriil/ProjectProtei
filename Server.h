#ifndef SERVER_H
#define SERVER_H
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <QTime>
#include "Callprocessing.h"
#include "Configjson.h"
#include <queue>
#include <mutex>


//ПОПРАВИТЬ ПАБЛИК ПРАЙВАТ
//ВЫНЕСТИ СЕРВЕР ВОРКЕР В ОТДЕЛЬНЫЙ КЛАСС
//Воркер описание зачем он нужен

//ДОБАВИТЬ ДЕСТРКУТОРЫ НА ВСЕ NEW ТРЕД НАДО ВРОДЕ ПРЕДВАРИТЕЛЬНО ОСТАНОВиТЬ

class ServerWorker: public QObject
{
    Q_OBJECT
public:
    QTimer* workerTimer;

    ServerWorker(int opNumber, int queueSize);//Это говно, передавать весь конфиг нужно
    std::queue<int> queue;
    const size_t queueMaxSize;
    const size_t opNumber;
    std::vector<CallProcessing> operators;


    void checkQueue(int number); //У СHeck cqueue  и operatorsAssign общие данные - надо добавить мьютекс
public slots:
     void operatorsAssign();
     void startWorker();


};

class Server: public QObject
{
    Q_OBJECT
public:
    ServerWorker* serverWorker;
    QTimer* workerTimer;

    explicit Server(int opNumber, int queueSize);
    bool isRunning = false;

    double createID(long phoneNumber);
    void handleRequest(boost::beast::http::request<boost::beast::http::string_body>& request, boost::asio::ip::tcp::socket& socket, long num);


signals:
    void assignOp();//Вообще используется

public slots:
    void runServer();

private:
    QThread* checkQueryThread;//описание

};



#endif // SERVER_H
