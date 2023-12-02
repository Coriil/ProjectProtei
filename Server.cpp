#include "Server.h"


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
     //connect(serverWorker &Server::finishAnsweredCall, cdrWorker, &CDRWorker::recFinishAnsweredCall);
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
long Server::createID(long phoneNumber)//ПОЧЕМУ ОТРИЦАТЕЛЬНЫЕ АААА
{
    long msecCur = QTime(0, 0, 0).msecsTo(QTime::currentTime());//время (мсек) с начала дня до момента прихода заявки
    long ID = phoneNumber^msecCur;//операция XOR для создания ID из номера и времени
    return ID;
}


void Server::handleRequest(boost::beast::http::request<boost::beast::http::string_body>& request, boost::asio::ip::tcp::socket& socket, long num) {
    namespace http = boost::beast::http;
    // подготовка ответа
    http::response<http::string_body> response;
    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::server, "My HTTP Server");
    //response.set(http::field::content_type, "text/plain");
    QDateTime curDT = QDateTime::currentDateTime();
    long id= createID(num);
    response.body() = std::to_string(id);//в теле ответа содержится CallID
    response.prepare_payload();
    boost::beast::http::write(socket, response);// отправка ответа клиенту
    emit inCall(curDT,id,num);
}

void Server::handleRequestOverload(boost::beast::http::request<boost::beast::http::string_body> &request, boost::asio::ip::tcp::socket &socket, long number)
{
    namespace http = boost::beast::http;
    // подготовка ответа
    http::response<http::string_body> response;
    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::server, "My HTTP Server");
    //response.set(http::field::content_type, "text/plain");
    response.body() = "error: server is overloaded";//в случае перегрузки сервера
    response.prepare_payload();
    // отправка ответа клиенту
    boost::beast::http::write(socket, response);
    QDateTime curDT = QDateTime::currentDateTime();
    long id = createID(number);
    emit overload(curDT,id,number);
}

void Server::handleIncorrectRequest(boost::beast::http::request<boost::beast::http::string_body> &request, boost::asio::ip::tcp::socket &socket)
{
    namespace http = boost::beast::http;
    // подготовка ответа
    http::response<http::string_body> response;
    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::server, "My HTTP Server");
    response.set(http::field::content_type, "text/plain");
    response.body() = "error:incorrect number";//в случае перегрузки сервера
    response.prepare_payload();
    // отправка ответа клиенту
    boost::beast::http::write(socket, response);
}


void Server::handleCallDuplication(boost::beast::http::request<boost::beast::http::string_body> &request, boost::asio::ip::tcp::socket &socket)
{
    namespace http = boost::beast::http;
    // подготовка ответа
    http::response<http::string_body> response;
    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::server, "My HTTP Server");
    response.set(http::field::content_type, "text/plain");
    response.body() = "error: call duplication - already in queue ";//в случае перегрузки сервера
    response.prepare_payload();
    // отправка ответа клиенту
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

           boost::beast::flat_buffer buffer;
           http::request<http::string_body> request;
           boost::beast::http::read(socket, buffer, request);
           if(request.method() == http::verb::post) {//?
               qDebug() << "Request getted";
               std::string requestBody = request.body();
               std::string numberString = requestBody;//.substr();//
               long number=0;
               try{
                   number = std::stol(numberString);
               }
               catch(const std::invalid_argument){
                  handleIncorrectRequest(request, socket);
               }
               if (number>0 &&(std::to_string(number).size()==10))//проверка - номер состоит из 10 цифр
                {
                   switch(serverWorker->checkQueue(number))
                   {
                    case 0://вызов помещается в очередь
                        handleRequest(request, socket, number);
                        break;
                    case -1://перегрузка - очередь полная
                        handleRequestOverload(request, socket, number);
                        break;
                    case -2://дублирование вызова
                        handleCallDuplication(request, socket);
                       break;
                    default:
                        handleIncorrectRequest(request, socket);
                   }
               }
               else
                    handleIncorrectRequest(request, socket);         
           }
           else {
               handleIncorrectRequest(request, socket);

           }
           // Close the socket
           socket.shutdown(tcp::socket::shutdown_send);
       }
}





