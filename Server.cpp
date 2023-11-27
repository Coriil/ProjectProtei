#include "Server.h"


Server::Server(ConfigJson cfg)
{
     isRunning=true;
     checkQueryThread = new QThread();
     serverWorker = new ServerWorker(cfg);
     serverWorker->moveToThread(checkQueryThread);
     connect(checkQueryThread, &QThread::started, serverWorker, &ServerWorker::startWorker);
     checkQueryThread->start();
}

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;//ДОЛЖНО БЫТЬ В НАЧАЛЕ ФАЙЛА ИЛИ В ФУНКЦИИИ В НАЧАЛЕ А НЕ В СЕРЕДИНЕ


long Server::createID(long phoneNumber)
{
    long msecCur = QTime(0, 0, 0).msecsTo(QTime::currentTime());
    long ID =phoneNumber^msecCur;
    return ID;
}


void Server::handleRequest(boost::beast::http::request<boost::beast::http::string_body>& request, boost::asio::ip::tcp::socket& socket, long num) {
    namespace http = boost::beast::http;
    // Prepare the reАsponse message
    http::response<http::string_body> response;
    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::server, "My HTTP Server");
    response.set(http::field::content_type, "text/plain");
    response.body() = std::to_string(createID(num));
    response.prepare_payload();

    // Send the response to the client
    boost::beast::http::write(socket, response);
}



void Server::runServer() {
    using tcp = boost::asio::ip::tcp;
    namespace http = boost::beast::http;
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
               long number = std::stoll(numberString);//DOUBLE ХОТЯ iNT СДЕЛАТЬ ПРОВЕРКУ stoll
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





