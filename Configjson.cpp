#include "Configjson.h"
#include <QFile>
#include <QDebug>

ConfigJson::ConfigJson()
{
    setDefaultValues(); //выставляем значения по умолчанию
}

ConfigJson::ConfigJson(QString filePath)
{
    if(readConfigJSON(filePath) != 0 )
    {
        setDefaultValues(); //ошибка при считывании конфига - выставляем значения по умолчанию
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

    if (checkValue(jsonObject, "queue size") == 0)
        cfg.queueSize = jsonObject.value("queue size").toInt(-1);
    else
        return -1;

    if (checkValue(jsonObject, "operators number") == 0)
        cfg.opNumber = jsonObject.value("operators number").toInt(-1);
    else
        return -1;

    if (checkValue(jsonObject, "minimal time") == 0)
        cfg.timeRmin= jsonObject.value("minimal time").toInt(-1);
    else
        return -1;

    if (checkValue(jsonObject, "maxmum time") == 0)
        cfg.timeRmax = jsonObject.value("maxmum time").toInt(-1);
    else
        return -1;


    return 0;
}

int ConfigJson::checkValue(QJsonObject &jsonObject, QString valName)
{
    if (jsonObject.value(valName) == QJsonValue::Undefined) // проверка что значение существует
        return -1;

    int ans = jsonObject.value(valName).toInt(-1); // проверка что значение Int
    if (ans == -1)
        return -1;

    return 0;
}


void ConfigJson::setDefaultValues()
{
    cfg.queueSize = 10; //размер очереди
    cfg.opNumber = 5; //число операторов
    cfg.timeRmin = 100; //минимальное
    cfg.timeRmax = 1000; // и максимальное время ожидания (секунды)
}
