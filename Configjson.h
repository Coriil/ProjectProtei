#ifndef CONFIGJSON_H
#define CONFIGJSON_H

#include <QJsonDocument>
#include <QJsonObject>

class ConfigJson
{
private:
    struct ConfData
    {
        int queueSize; //размер очереди
        int opNumber; //число операторов
        int timeRmin; //минимальное
        int timeRmax; // и максимальное время ожидания (секунды)
    //добавить: мин и макс время обслуживания, вариант обработки дублитрующих вызовов*
    };
    ConfData cfg;
public:
    ConfigJson();
    ConfigJson(QString filePath);


    int getQueueSize(){return cfg.queueSize;}
    int getOpNumber(){return cfg.opNumber;}
    int getTimeRmin(){return cfg.timeRmin;}
    int getTimeRmax(){return cfg.timeRmax;}

    int readConfigJSON(QString filePath);
    void setDefaultValues();
    int checkValue(QJsonObject &jsonObject, QString valName);
};

#endif // CONFIGJSON_H
