#include <Configjson.h>
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
        qDebug() << "config file not found - default values set";
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

    return validateJson(jsonObject);
}

//проверка и считывание всех значений в файле
int ConfigJson::readConfigJSON(QJsonObject &obj)
{
    return validateJson(obj);
}

//проверка и считывание всех значений в файле
int ConfigJson::validateJson(QJsonObject &jsonObject)
{
    if ((checkValue(jsonObject, "queue size") == 0)&&(jsonObject.value("queue size").toInt(-1)>1))//минимальный размер очереди равен 1
        cfgData.queueSize = jsonObject.value("queue size").toInt(-1);
    else
        return -1;

    if ((checkValue(jsonObject, "operators number") == 0)&&(jsonObject.value("operators number").toInt(-1)>1))//минимальное число операторов равно 1
        cfgData.opNumber = jsonObject.value("operators number").toInt(-1);
    else
        return -1;

    if (checkValue(jsonObject, "wait min time sec") == 0)
        cfgData.timeRminSec= jsonObject.value("wait min time sec").toInt(-1);
    else
        return -1;

    if (checkValue(jsonObject, "wait max time sec") == 0)
        cfgData.timeRmaxSec = jsonObject.value("wait max time sec").toInt(-1);
    else
        return -1;
    if (cfgData.timeRminSec>cfgData.timeRmaxSec)//неправильные пределы времени - минимальное больше максимального
    {
        return -2;
    }
    if (checkValue(jsonObject, "operator busy min time sec") == 0)
        cfgData.timeOpMinSec = jsonObject.value("operator busy min time sec").toInt(-1);
    else
        return -1;

    if (checkValue(jsonObject, "operator busy max time sec") == 0)
        cfgData.timeOpMaxSec = jsonObject.value("operator busy max time sec").toInt(-1);
    else
        return -1;

    if (cfgData.timeOpMinSec>cfgData.timeOpMaxSec)//неправильные пределы времени - минимальное больше максимального
    {
        return -2;
    }
    return 0;
}


//проверка одного значения
int ConfigJson::checkValue(QJsonObject &jsonObject, QString valName)
{
    if (jsonObject.value(valName) == QJsonValue::Undefined) // проверка что значение существует
        return -1;

    int ans = jsonObject.value(valName).toInt(-1); // проверка что значение - число
    if (ans == -1)
        return -1;
    if (ans<0) // проверка что число положительное, строго больше нуля
        return -1;
    return 0;
}

//установка значений по умолчанию
void ConfigJson::setDefaultValues()
{
    cfgData.queueSize = 20; //размер очереди
    cfgData.opNumber = 5; //число операторов
    cfgData.timeRminSec = 100; //минимальное
    cfgData.timeRmaxSec = 1000; // и максимальное время ожидания (секунды)
    cfgData.timeOpMinSec = 30; // минимальное
    cfgData.timeOpMaxSec = 120;// и максимальное время занятости оператора (секунды)
}
