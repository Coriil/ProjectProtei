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
        BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::warning)<< "configuration default value set";
    }
}


//чтение файла и сохранение настроек
jsonErrors ConfigJson::readConfigJSON(QString filePath)
{
    QFile file(filePath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::warning) << "could not open configutarion file for reading";
        return OPEN_FILE_ERR;
    }

    QByteArray jsonData = file.readAll();
    file.close();
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if(parseError.error != QJsonParseError::NoError) {
        BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::warning) << "Error parsing JSON: "<<  parseError.errorString().toStdString();
        return PARSE_ERROR;
    }

    QJsonObject jsonObject = jsonDoc.object();
    return validateJson(jsonObject);
}

//проверка и считывание всех значений в файле
jsonErrors ConfigJson::readConfigJSON(QJsonObject &obj)
{
    return validateJson(obj);
}

//проверка и считывание всех значений в файле
jsonErrors ConfigJson::validateJson(QJsonObject &jsonObject)
{
    if ((checkValue(jsonObject, "queue size") == 0)&&(jsonObject.value("queue size").toInt(-1)>1))//минимальный размер очереди равен 1
        cfgData.queueSize = jsonObject.value("queue size").toInt(-1);
    else
        return KEY_ERROR;

    if ((checkValue(jsonObject, "operators number") == 0)&&(jsonObject.value("operators number").toInt(-1)>1))//минимальное число операторов равно 1
        cfgData.opNumber = jsonObject.value("operators number").toInt(-1);
    else
        return KEY_ERROR;

    if (checkValue(jsonObject, "wait min time sec") == 0)
        cfgData.timeRminSec= jsonObject.value("wait min time sec").toInt(-1);
    else
        return KEY_ERROR;

    if (checkValue(jsonObject, "wait max time sec") == 0)
        cfgData.timeRmaxSec = jsonObject.value("wait max time sec").toInt(-1);
    else
        return KEY_ERROR;
    if (cfgData.timeRminSec>cfgData.timeRmaxSec)//неправильные пределы времени - минимальное больше максимального
    {
        return VALUE_ERROR;
        BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::warning) << "wait time limits min/max not correct";
    }
    if (checkValue(jsonObject, "operator busy min time sec") == 0)
        cfgData.timeOpMinSec = jsonObject.value("operator busy min time sec").toInt(-1);
    else
        return KEY_ERROR;

    if (checkValue(jsonObject, "operator busy max time sec") == 0)
        cfgData.timeOpMaxSec = jsonObject.value("operator busy max time sec").toInt(-1);
    else
        return KEY_ERROR;

    if (cfgData.timeOpMinSec>cfgData.timeOpMaxSec)//неправильные пределы времени - минимальное больше максимального
    {
        return VALUE_ERROR;
        BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::warning) << "operator time limits min/max not correct";
    }
    return OK;
}


//проверка одного значения
jsonErrors ConfigJson::checkValue(QJsonObject &jsonObject, QString valName)
{
    if (jsonObject.value(valName) == QJsonValue::Undefined) // проверка что значение существует
        return KEY_ERROR;

    int ans = jsonObject.value(valName).toInt(-1); // проверка что значение - число
    if (ans == -1)
    {
        return VALUE_ERROR;
        BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::warning) << valName.toStdString() << " value is not an integer";
    }
    if (ans<0) // проверка что число положительное, строго больше нуля
    {
        return VALUE_ERROR;
        BOOST_LOG_SEV(my_logger::get(),boost::log::trivial::warning) << valName.toStdString() << " value is negative number";
    }
    return OK;
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
