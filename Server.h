#ifndef SERVER_H
#define SERVER_H
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <QTime>
#include "Callprocessing.h"
#include "Configjson.h"
#include <queue>
#include <mutex>
#include "Serverworker.h"


//ПОПРАВИТЬ ПАБЛИК ПРАЙВАТ
//ВЫНЕСТИ СЕРВЕР ВОРКЕР В ОТДЕЛЬНЫЙ КЛАСС+
//Воркер описание зачем он нужен

//ДОБАВИТЬ ДЕСТРКУТОРЫ НА ВСЕ NEW ТРЕД НАДО ВРОДЕ ПРЕДВАРИТЕЛЬНО ОСТАНОВиТЬ


class Server: public QObject
{
private:
    QThread* checkQueryThread;//описание
    Q_OBJECT
public:
    ServerWorker* serverWorker;//
    QTimer* workerTimer;

    explicit Server(ConfigJson cfg);
    bool isRunning = false;

    long createID(long phoneNumber);
    void handleRequest(boost::beast::http::request<boost::beast::http::string_body>& request, boost::asio::ip::tcp::socket& socket, long num);

signals:
    void assignOp();//Вообще используется

public slots:
    void runServer();



};



#endif // SERVER_H
