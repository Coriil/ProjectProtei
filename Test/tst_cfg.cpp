#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <Configjson.h>

using namespace testing;

class MyClassTest : public Test {
protected:
     ConfigJson cfg;
};

TEST(ConfigjsonTest, noCfgFile)
{
    ConfigJson cfg;
    int a = cfg.readConfigJSON("no file");
    ASSERT_NE(a, 0);

}

TEST(ConfigjsonTest, JsonObjectNoSuchValue)
{
    ConfigJson cfg;
    QJsonObject jsObj =
    {
        {"property1", 1},
        {"property2", 2}
    };
    //int answ = cfg.checkValue(jsObj, "property1");
}
/*
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
}*/
