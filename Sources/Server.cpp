#include <Server.h>

namespace http = boost::beast::http;
using tcp = boost::asio::ip::tcp;

Server::Server(ConfigJson cfg)
{
     isRunning=true;
     checkQueryThread = new QThread(); //создается новый поток для обработки очереди
     serverWorker = new ServerWorker(cfg);//класс ServerWorker для обработки очереди заявок
     serverWorker->moveToThread(checkQueryThread);//работа с очередью происходит в новом потоке, отдельном от потока приёма заявок
     connect(checkQueryThread, &QThread::started, serverWorker, &ServerWorker::startWorker);
     cdrThread = new QThread();
     cdrWorker = new CDRWorker();
     cdrWorker -> moveToThread(cdrThread);
     connect(cdrThread, &QThread::started, cdrWorker, &CDRWorker::startCDR);
     connect(this, &Server::inCall, cdrWorker, &CDRWorker::recInCall);
     connect(this, &Server::answerCall, cdrWorker, &CDRWorker::recAnswerCall);
     connect(serverWorker, &ServerWorker::answerCall, cdrWorker, &CDRWorker::recAnswerCall);
     connect(serverWorker, &ServerWorker::finAnswerCall, cdrWorker, &CDRWorker::recFinishAnsweredCall);
     //connect(serverWorker, &ServerWorker::timeoutedCalls, cdrWorker, &CDRWorker::recTimeoutedCalls);
     connect(this, &Server::callDuplication, cdrWorker, &CDRWorker::recCallDuplication);
     connect(this, &Server::overload, cdrWorker, &CDRWorker::recCallOverload);
     checkQueryThread->start();
     cdrThread ->start();
}

Server::~Server()
{
    delete checkQueryThread;
    delete cdrThread;
    delete serverWorker;
}


//создание идентификатора заявки CallID
long Server::createID(long phoneNumber)
{
    long msecCur = QTime(0, 0, 0).msecsTo(QTime::currentTime());//время (мсек) с начала дня до момента прихода заявки
    long ID = phoneNumber^msecCur;//операция XOR для создания ID из номера и времени
    return ID;
}

//обработка запросов (случаи помещения запроса в очередь, перегрузки при заполненной очереди, дублирования вызовов)
void Server::handleRequest(http::request<http::string_body>& request, boost::asio::ip::tcp::socket& socket, long num, long id, WorkerStatus status) {
    // подготовка ответа
    http::response<http::string_body> response;
    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::server, "My HTTP Server");
    response.set(http::field::content_type, "text/plain");
    switch (status) {
    case WorkerStatus::DEFAULT://неправильный формат номера
    {
        response.body() = "error:incorrect number";
        response.prepare_payload();
        http::write(socket, response);
    }
    break;
    case WorkerStatus::OK://вызов помещается в очередь
    {
        QDateTime curDT = QDateTime::currentDateTime();
        response.body() = std::to_string(id);//в теле ответа содержится CallID
        response.prepare_payload();
        http::write(socket, response);// отправка ответа клиенту
        emit inCall(curDT,id,num);
    }
    break;
    case WorkerStatus::OVERLOAD://в случае перегрузки сервера
    {
        response.body() = "error: server is overloaded";
        response.prepare_payload();
        http::write(socket, response);
        QDateTime curDT = QDateTime::currentDateTime();
        emit overload(curDT,id, num);
    }
    break;
    case WorkerStatus::DUPLICATE://дублирование вызова
    {
        response.body() = "error: call duplication - already in queue ";
        response.prepare_payload();
        http::write(socket, response);
        QDateTime curDT = QDateTime::currentDateTime();
        emit callDuplication(curDT, id, num);
    }
    break;
    }

}


//запуск сервера
void Server::runServer() {
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, {tcp::v4(), 8080});
       while (true) {
           tcp::socket socket(io_context);
           acceptor.accept(socket);
           boost::beast::flat_buffer buffer;
           http::request<http::string_body> request;
           http::read(socket, buffer, request);
           if(request.method() == http::verb::post) {
               qDebug() << "Request getted";
               std::string requestBody = request.body();
               std::string numberString = requestBody;
               long number=0;
               try{
                   number = std::stol(numberString);
               }
               catch(const std::invalid_argument){
                  handleRequest(request, socket);
               }
               if (number>0 &&(std::to_string(number).size()==10))//проверка - номер состоит из 10 цифр
                {
                   long id = createID(number);
                   WorkerStatus status = serverWorker->checkQueue(number, id);
                   handleRequest(request, socket, number, id, status);
               }
               else
                    handleRequest(request, socket);
           }
           else {
               handleRequest(request, socket);
           }
           socket.shutdown(tcp::socket::shutdown_send);
       }
}





