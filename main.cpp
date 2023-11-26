#include <QCoreApplication>
#include <iostream>
#include "Configjson.h"
#include "Callprocessing.h"
#include "Server.h"
#include <QDebug>
#include <vector>
#include <thread>
#include <QThread>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //ConfigJson conf("//home/anatoly/project");
    ConfigJson conf("/home/polina/config.json");
    //QThread *thread2=new QThread();
   // Op->moveToThread(thread2);

    //vector<CallProcessing> callingOperators = vector<CallProcessing>(conf.getOpNumber());
    Server *srv = new Server(conf.getOpNumber(), conf.getQueueSize());

    QThread *thread1=new QThread();
    srv->moveToThread(thread1);
    QObject::connect(thread1, SIGNAL(started()), srv, SLOT(runServer()));
    try {
            thread1->start();
    }
    catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
    }
    return a.exec();
}


/*ConfigJson conf;
//conf.ReadConfigJSON();
 std::string configString = std::to_string(conf.getOpNumber()) + " " + std::to_string(conf.getQueueSize())
        + " " + std::to_string(conf.getTimeRmax())
        + " " + std::to_string(conf.getTimeRmin());
response.body() = configString;*/
