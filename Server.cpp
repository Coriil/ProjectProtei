#include "Server.h"



Server::Server(int opNumber, int queueSize) : queueMaxSize(queueSize)
{
     isRunning=true;
     operators = std::vector<CallProcessing>(opNumber);

     for (size_t i = 0; i < operators.size(); ++i)
     {
         operators[i].setNumber(i);
     }
}

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;


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
    response.body() = std::to_string(createID(num));
    response.prepare_payload();

    // Send the response to the client
    boost::beast::http::write(socket, response);
}

void Server::operatorsAssign() {
    for (size_t i = 0; i < operators.size(); ++i)
    {
        if (operators[i].m_isBusy == false && queue.empty() == false)
        {
            int number = queue.front();
            operators[i].m_isBusy = true;
            operators[i].computeData(number);
            qDebug() << "Queue size"<<queue.size();
            queue.pop();
        }
    }
}

void Server::checkQueue(int number) {
    if (queue.size() >= queueMaxSize)
    {
        qDebug() <<"Queue is full";
    }
    else
        queue.push(number);
}

void Server::runServer() {

    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, {tcp::v4(), 8080});

    while (true) {
        operatorsAssign();

        tcp::socket socket(io_context);
        acceptor.accept(socket);
        boost::system::error_code ec;
        //socket.wait(boost::asio::ip::tcp::socket::wait_read, ec);
        socket.wait(tcp::socket::wait_read, std::chrono::milliseconds(100));
        // Read the HTTP request
        boost::beast::flat_buffer buffer;
        http::request<http::string_body> request;
        boost::beast::http::read(socket, buffer, request);

        // Handle the request
        if(request.method() == http::verb::post) {
            //QTime callIn = QTime::currentTime();
            // Определяем, что это POST-запрос
            // Извлекаем данные из запроса (body)
            std::string requestBody = request.body();
            // Здесь вы можете извлечь номер из requestBody и передать его в handleRequest
            // Например, если номер находится в строке "number=12345678901", то можно извлечь его так:
            std::string numberString = requestBody.substr(requestBody.find("number=") + strlen("number="));
            // Преобразуем строку в число
            double number = std::stoll(numberString);
            handleRequest(request, socket, number);

            checkQueue(number);//checkQueue


        }
        else {
            // Это не POST-запрос
            //handleRequest(request, socket);
        }
        // Close the socket
        socket.shutdown(tcp::socket::shutdown_send);
    }
}





