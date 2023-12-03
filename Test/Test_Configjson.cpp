#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <Configjson.h>

using namespace testing;


TEST(ConfigjsonTest, noCfgFile)
{
    ConfigJson cfg;
    int a = cfg.readConfigJSON("no file");
    ASSERT_NE(a, 0);

}

TEST(ConfigjsonTest, JsonObjectIsCorrect)
{
    ConfigJson cfg;
    QJsonObject jsObj =
    {
        {"queue size", 3},
        {"operators number", 2},
        {"minimal time", 5},
        {"maxmum time", 7},
        {"busyness min time", 20},
        {"busyness max time", 50}
    };
    int answ = cfg.readConfigJSON(jsObj);
    ASSERT_EQ (answ, 0);
}

TEST(ConfigjsonTest, JsonValueIsNotNumber)
{
    ConfigJson cfg;
    QJsonObject jsObj =
    {
        {"queue size", "3"},
        {"operators number", 2},
        {"minimal time", 5},
        {"maxmum time", 7},
        {"busyness min time", 20},
        {"busyness max time", 50}
    };
    int answ = cfg.readConfigJSON(jsObj);
    ASSERT_NE (answ, 0);
}

TEST(ConfigjsonTest, JsonMissingValue)
{
    ConfigJson cfg;
    QJsonObject jsObj =
    {
        {"operators number", 2},
        {"minimal time", 5},
        {"maxmum time", 7},
        {"busyness min time", 20},
        {"busyness max time", 50}
    };
    int answ = cfg.readConfigJSON(jsObj);
    ASSERT_NE (answ, 0);
}

TEST(ConfigjsonTest, JsonCheckZeroValue)
{
    ConfigJson cfg;
    QJsonObject jsObj =
    {
        {"queue size", 0},
        {"operators number", 2},
        {"minimal time", 5},
        {"maxmum time", 7},
        {"busyness min time", 20},
        {"busyness max time", 50}
    };
    int answ = cfg.readConfigJSON(jsObj);
    ASSERT_NE (answ, 0);
}

TEST(ConfigjsonTest, JsonCheckNegativeValue)
{
    ConfigJson cfg;
    QJsonObject jsObj =
    {
        {"queue size", -1},
        {"operators number", 2},
        {"minimal time", 5},
        {"maxmum time", 7},
        {"busyness min time", 20},
        {"busyness max time", 50}
    };
    int answ = cfg.readConfigJSON(jsObj);
    ASSERT_NE (answ, 0);
}

TEST(ConfigjsonTest, JsonCheckMinMaxIncorrect)
{
    ConfigJson cfg;
    QJsonObject jsObj =
    {
        {"queue size", 2},
        {"operators number", 2},
        {"minimal time", 5},
        {"maxmum time", 7},
        {"busyness min time", 50},
        {"busyness max time", 30}
    };
    int answ = cfg.readConfigJSON(jsObj);
    ASSERT_NE (answ, 0);
}
