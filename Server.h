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
#include "CDRWorker.h"


//ПОПРАВИТЬ ПАБЛИК ПРАЙВАТ
//Воркер описание зачем он нужен
//ДОБАВИТЬ ДЕСТРКУТОРЫ НА ВСЕ NEW ТРЕД НАДО ВРОДЕ ПРЕДВАРИТЕЛЬНО ОСТАНОВиТЬ


class Server: public QObject
{
private:
    QThread* checkQueryThread;//описание
    Q_OBJECT
public:

    explicit Server(ConfigJson cfg);
    ~Server();
    ServerWorker* serverWorker;//

    bool isRunning = false;
    long createID(long phoneNumber);
    void handleRequest(boost::beast::http::request<boost::beast::http::string_body>& request, boost::asio::ip::tcp::socket& socket, long num);//мда
    void handleRequestOverload(boost::beast::http::request<boost::beast::http::string_body>& request, boost::asio::ip::tcp::socket& socket);
    void handleIncorrectRequest(boost::beast::http::request<boost::beast::http::string_body>& request, boost::asio::ip::tcp::socket& socket);;

signals:
    void assignOp();//Вообще используется

public slots:
    void runServer();



};



#endif // SERVER_H
