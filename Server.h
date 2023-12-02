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
    QThread* cdrThread;
    Q_OBJECT
public:

    explicit Server(ConfigJson cfg);
    ~Server();
    ServerWorker* serverWorker;//
    CDRWorker* cdrWorker;
    bool isRunning = false;
    long createID(long phoneNumber);
    void handleRequest(boost::beast::http::request<boost::beast::http::string_body>& request, boost::asio::ip::tcp::socket& socket, long num);
    void handleRequestOverload(boost::beast::http::request<boost::beast::http::string_body>& request, boost::asio::ip::tcp::socket& socket,long num);
    void handleIncorrectRequest(boost::beast::http::request<boost::beast::http::string_body>& request, boost::asio::ip::tcp::socket& socket);
    void handleCallDuplication(boost::beast::http::request<boost::beast::http::string_body>& request, boost::asio::ip::tcp::socket& socket, long num);
signals:
    void assignOp();//Вообще используется
    void inCall(QDateTime inCall, long ID, long phNumber);//входящий вызов
    void answerCall(QDateTime ansDT, int opNum, long ID);//ответ оператора на вызов
    void finishAnsweredCall(QDateTime finishDT, long ID);//окончание ответа опреатора
    void overload(QDateTime ansDT,long ID,int opNum);
    void callDuplication(QDateTime ansDT,long ID,int opNum);
public slots:
    void runServer();



};



#endif // SERVER_H
