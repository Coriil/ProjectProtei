#ifndef CONFIGJSON_H
#define CONFIGJSON_H

#include <QJsonDocument>
#include <QJsonObject>
#include<Mylogger.h>


enum jsonErrors
{
    OPEN_FILE_ERR = -1,
    PARSE_ERROR = -2,
    KEY_ERROR = -3,
    VALUE_ERROR =-4,
    OK = 0
};
//класс для чтения json-файла конфигурации сервера и проверки корректности полей этого файла
class ConfigJson
{
private:

    struct ConfData
    {
        int queueSize; //размер очереди
        int opNumber; //число операторов
        int timeRminSec; //минимальное
        int timeRmaxSec; // и максимальное время ожидания в очереди (секунды)
        int timeOpMinSec; // минимальное
        int timeOpMaxSec;//и максимальное время занятости оператора (секунды)
    };
    ConfData cfgData;
    void setDefaultValues();
    jsonErrors checkValue(QJsonObject &jsonObject, QString valName);
    jsonErrors validateJson(QJsonObject &jsonObject);

public:
    ConfigJson();
    ConfigJson(QString filePath);
    ConfData getCfg(){return cfgData;};
    int getQueueSize(){return cfgData.queueSize;}
    int getOpNumber(){return cfgData.opNumber;}
    int getWaitTimeMin(){return cfgData.timeRminSec;}
    int getWaitTimeMax(){return cfgData.timeRmaxSec;}
    int getOpTimeMin(){return cfgData.timeOpMinSec;}
    int getOpTimeMax(){return cfgData.timeOpMaxSec;}
    jsonErrors readConfigJSON(QString filePath);
    jsonErrors readConfigJSON(QJsonObject &obj);


};

#endif // CONFIGJSON_H
