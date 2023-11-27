#include "Configjson.h"
#include <QFile>
#include <QDebug>

ConfigJson::ConfigJson()
{
    setDefaultValues(); //если пусть к файлу конфигурации не указан - выставляются знаечния по умолчанию
}

ConfigJson::ConfigJson(QString filePath)
{
    if(readConfigJSON(filePath) != 0 )//попытка чтения файла конфигурации
    {
        setDefaultValues(); //ошибка при считывании конфигурации - выставляются занчения по умолчанию
    }
}


//чтение файла и сохранение настроек
int ConfigJson::readConfigJSON(QString filePath)
{
    QFile file(filePath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open the file for reading";
        return -1;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if(parseError.error != QJsonParseError::NoError) {
        qDebug() << "Error parsing JSON: " << parseError.errorString();
        return -1;
    }

    QJsonObject jsonObject = jsonDoc.object();

    if ((checkValue(jsonObject, "queue size") == 0)&&(jsonObject.value("queue size").toInt(-1)>1))//
        cfgData.queueSize = jsonObject.value("queue size").toInt(-1);
    else
        return -1;

    if (checkValue(jsonObject, "operators number") == 0)
        cfgData.opNumber = jsonObject.value("operators number").toInt(-1);
    else
        return -1;

    if (checkValue(jsonObject, "minimal time") == 0)
        cfgData.timeRmin= jsonObject.value("minimal time").toInt(-1);
    else
        return -1;

    if (checkValue(jsonObject, "maxmum time") == 0)
        cfgData.timeRmax = jsonObject.value("maxmum time").toInt(-1);
    else
        return -1;

    if (checkValue(jsonObject, "busyness min time") == 0)
        cfgData.timeOpMin = jsonObject.value("busyness min time").toInt(-1);
    else
        return -1;

    if (checkValue(jsonObject, "busyness max time") == 0)
        cfgData.timeOpMax = jsonObject.value("busyness max time").toInt(-1);
    else
        return -1;

    if (cfgData.timeOpMin>=cfgData.timeOpMax)//
    {
        return -2;
    }


    return 0;
}

//проверка занчений
int ConfigJson::checkValue(QJsonObject &jsonObject, QString valName)
{
    if (jsonObject.value(valName) == QJsonValue::Undefined) // проверка что значение существует
        return -1;

    int ans = jsonObject.value(valName).toInt(-1); // проверка что значение - число
    if (ans == -1)
        return -1;
    if (ans<0) // проверка что число положительное
        return -1;
    return 0;
}


void ConfigJson::setDefaultValues()
{
    cfgData.queueSize = 10; //размер очереди
    cfgData.opNumber = 5; //число операторов
    cfgData.timeRmin = 100; //минимальное
    cfgData.timeRmax = 1000; // и максимальное время ожидания (секунды)
    cfgData.timeOpMin = 20; // минимальное
    cfgData.timeOpMax = 50;// и максимальное время занятости оператора (секунды)
}
