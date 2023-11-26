#include "Server.h"


ServerWorker::ServerWorker(int opNumber, int queueSize) : queueMaxSize(queueSize), opNumber(opNumber) {

}

void ServerWorker::startWorker() {
    operators = std::vector<CallProcessing>(opNumber);
    for (size_t i = 0; i < operators.size(); ++i)
    {
        operators[i].setNumber(i);
    }

    workerTimer = new QTimer();
    connect(workerTimer, &QTimer::timeout, this, &ServerWorker::operatorsAssign);
    workerTimer->start(1000); // Вызываем функцию каждsые N мс
}

void ServerWorker::operatorsAssign() {
         for (size_t i = 0; i < operators.size(); ++i)
        {
            if (operators[i].m_isBusy == false && queue.empty() == false)
            {
                qDebug() <<"Operators assign";
                int number = queue.front();
                operators[i].m_isBusy = true;
                operators[i].computeData(number);
                qDebug() << "Queue size"<<queue.size();
                queue.pop();
            }
        }
}

void ServerWorker::checkQueue(int number) {
    qDebug() <<"Check queue";
    if (queue.size() >= queueMaxSize)
    {
        qDebug() <<"Queue is full";
    }
    else
        queue.push(number);
}




Server::Server(int opNumber, int queueSize)
{
     isRunning=true;
     checkQueryThread = new QThread();
     serverWorker = new ServerWorker(opNumber, queueSize);

     serverWorker->moveToThread(checkQueryThread);
     connect(checkQueryThread, &QThread::started, serverWorker, &ServerWorker::startWorker);
     checkQueryThread->start();

 }

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;//ДОЛЖНО БЫТЬ В НАЧАЛЕ ФАЙЛА ИЛИ В ФУНКЦИИИ В НАЧАЛЕ А НЕ В СЕРЕДИНЕ


double Server::createID(long phoneNumber)
{
    long msecCur = QTime(0, 0, 0).msecsTo(QTime::currentTime());
    double ID = phoneNumber^msecCur;
    return ID;
}


void Server::handleRequest(boost::beast::http::request<boost::beast::http::string_body>& request, boost::asio::ip::tcp::socket& socket, long num) {
    // Prepare the response message
    http::response<http::string_body> response;
    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::server, "My HTTP Server");
    response.set(http::field::content_type, "text/plain");
    response.body() = std::to_string(createID(num));//DOUBLE ХОТЯ iNT
    response.prepare_payload();

    // Send the response to the client
    boost::beast::http::write(socket, response);
}



void Server::runServer() {
    boost::asio::io_context io_context;
       tcp::acceptor acceptor(io_context, {tcp::v4(), 8080});

       while (true) {

           tcp::socket socket(io_context);
           acceptor.accept(socket);
           boost::system::error_code ec;
           //socket.wait(boost::asio::ip::tcp::socket::wait_read, ec);
            // Read the HTTP request
           boost::beast::flat_buffer buffer;
           http::request<http::string_body> request;
           boost::beast::http::read(socket, buffer, request);

           // Handle the request
           if(request.method() == http::verb::post) {
               qDebug() << "Request getted";


               //QTime callIn = QTime::currentTime();
               // Определяем, что это POST-запрос
               // Извлекаем данные из запроса (body)
               std::string requestBody = request.body();
               // Здесь вы можете извлечь номер из requestBody и передать его в handleRequest
               // Например, если номер находится в строке "number=12345678901", то можно извлечь его так:
               std::string numberString = requestBody.substr(requestBody.find("number=") + strlen("number="));
               // Преобразуем строку в число
               double number = std::stoll(numberString);//DOUBLE ХОТЯ iNT СДЕЛАТЬ ПРОВЕРКУ stoll
               serverWorker->checkQueue(number);
               //ПРОВЕРКА ВОЗВРАТА СДЕЛАТЬ ЗАПРОС НАЗАД ЧТО НЕ ДОБАВИЛИИ


               handleRequest(request, socket, number);


           }
           else {
               // Это не POST-запрос
               //handleRequest(request, socket);
           }
           // Close the socket
           socket.shutdown(tcp::socket::shutdown_send);
       }
}





