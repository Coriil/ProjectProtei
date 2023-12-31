#ifndef SERVER_H
#define SERVER_H
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <Callprocessing.h>
#include <Configjson.h>
#include <Serverworker.h>
#include <CDRWorker.h>
#include <Caller.h>

namespace http = boost::beast::http;
using tcp = boost::asio::ip::tcp;

//основной класс программы - принимает http-запросы и формирует ответы
class Server: public QObject
{
    Q_OBJECT
private:
    QThread* checkQueryThread;//описание
    QThread* cdrThread;
    void handleRequest(http::request<http::string_body>& request, tcp::socket& socket, long num = 0, long id = 0, WorkerStatus status = WorkerStatus::DEFAULT);
    long createID(long phoneNumber);
    ServerWorker* serverWorker;
    CDRWorker* cdrWorker;

public:
    explicit Server(ConfigJson cfg);
    ~Server();
    bool isRunning = false;

signals:
    void inCall(QDateTime inCall, long ID, long phNumber);//входящий вызов
    void overload(QDateTime ansDT,long ID,int opNum);
    void callDuplication(QDateTime ansDT,long ID, long opNum);

public slots:
    void runServer();

};



#endif // SERVER_H
