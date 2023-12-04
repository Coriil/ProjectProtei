#include <QCoreApplication>
#include <Configjson.h>
#include <Server.h>
#include <QDebug>
#include <QThread>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ConfigJson conf("/home/polina/config.json");//чтение файла конфигурации
    Server *srv = new Server(conf);//создание сервера с настройками из файла конфигурации
    QThread *serverThread = new QThread();
    QObject::connect(serverThread, &QThread::finished, serverThread, &QThread::deleteLater);
    srv->moveToThread(serverThread);
    QObject::connect(serverThread, SIGNAL(started()), srv, SLOT(runServer()));
    try
    {
        serverThread->start();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return a.exec();
}
