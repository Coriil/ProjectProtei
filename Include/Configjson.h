#ifndef CONFIGJSON_H
#define CONFIGJSON_H

#include <QJsonDocument>
#include <QJsonObject>

//класс для чтения json-файла конфигурации сервера и проверки корректности полей этого файла
class ConfigJson
{
private:
    struct ConfData
    {
        int queueSize; //размер очереди
        int opNumber; //число операторов
        int timeRmin; //минимальное
        int timeRmax; // и максимальное время ожидания в очереди (секунды)
        int timeOpMin; // минимальное
        int timeOpMax;//и максимальное время занятости оператора (секунды)
    };
    ConfData cfgData;
    void setDefaultValues();
    int checkValue(QJsonObject &jsonObject, QString valName);
    int validateJson(QJsonObject &jsonObject);
public:
    ConfigJson();
    ConfigJson(QString filePath);
    ConfData getCfg(){return cfgData;};
    int getQueueSize(){return cfgData.queueSize;}
    int getOpNumber(){return cfgData.opNumber;}
    int getWaitTimeMin(){return cfgData.timeRmin;}
    int getWaitTimeMax(){return cfgData.timeRmax;}
    int getOpTimeMin(){return cfgData.timeOpMin;}
    int getOpTimeMax(){return cfgData.timeOpMax;}
    int readConfigJSON(QString filePath);
    int readConfigJSON(QJsonObject &obj);
};

#endif // CONFIGJSON_H
