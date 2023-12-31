#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <Configjson.h>

using namespace testing;


TEST(ConfigjsonTest, noCfgFile)
{
    ConfigJson cfg;
    jsonErrors a = cfg.readConfigJSON("no file");
    ASSERT_EQ(a, OPEN_FILE_ERR);

}

TEST(ConfigjsonTest, JsonObjectIsCorrect)
{
    ConfigJson cfg;
    QJsonObject jsObj =
    {
        {"queue size", 3},
        {"operators number", 2},
        { "wait min time sec", 5},
        {"wait max time sec", 7},
        {"operator busy min time sec", 20},
        {"operator busy max time sec", 50}

    };
    jsonErrors answ = cfg.readConfigJSON(jsObj);
    ASSERT_EQ (answ, OK);
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
    jsonErrors answ = cfg.readConfigJSON(jsObj);
    ASSERT_EQ (answ, KEY_ERROR);
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
    jsonErrors answ = cfg.readConfigJSON(jsObj);
    ASSERT_EQ (answ, KEY_ERROR);
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
    jsonErrors answ = cfg.readConfigJSON(jsObj);
    ASSERT_EQ (answ, KEY_ERROR);
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
    jsonErrors answ = cfg.readConfigJSON(jsObj);
    ASSERT_EQ (answ, KEY_ERROR);
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
    jsonErrors answ = cfg.readConfigJSON(jsObj);
    ASSERT_EQ (answ, KEY_ERROR);
}

