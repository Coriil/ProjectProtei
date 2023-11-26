#ifndef SERVER_H
#define SERVER_H
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <QTime>
#include "Callprocessing.h"
#include "Configjson.h"
#include <queue>
#include <mutex>


class Server: public QObject
{
public:

    explicit Server(int opNumber, int queueSize);
    bool isRunning = false;
    Q_OBJECT
    double createID(long phoneNumber);
    void handleRequest(boost::beast::http::request<boost::beast::http::string_body>& request, boost::asio::ip::tcp::socket& socket, long num);


signals:
    void assignOp();

public slots:
    void runServer();
private:
    std::vector<CallProcessing> operators;
    std::queue<int> queue;
    const size_t queueMaxSize;
    std::mutex mutex;  // Мьютекс для защиты очереди и вектора операторов

    void operatorsAssign();
    void checkQueue(int number);

};

#endif // SERVER_H
