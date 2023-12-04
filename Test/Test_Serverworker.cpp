#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <Serverworker.h>
#include <Configjson.h>

using namespace testing;

class ServerWorkerTest : public ::testing::Test {
protected:
    void SetUp() override {

        QJsonObject jsObj =
        {
            {"queue size", 2},
            {"operators number", 2},
            {"minimal time", 5},
            {"maxmum time", 7},
            {"busyness min time", 20},
            {"busyness max time", 50}
        };

        cfg.readConfigJSON(jsObj);
        srvWrk = new ServerWorker(cfg);
    }

    void TearDown() override {
        delete srvWrk;
    }

    ConfigJson cfg;
    ServerWorker *srvWrk = nullptr;
};

TEST_F(ServerWorkerTest, ServerWorkerOverload) {
    Caller caller1(1234567890, 1);
    Caller caller2(7894561230, 2);
    Caller caller3(4561237890, 3);
    caller1.setTimeoutDTSecs(5);
    caller2.setTimeoutDTSecs(5);
    caller3.setTimeoutDTSecs(5);
    srvWrk->checkQueue(caller1);
    srvWrk->checkQueue(caller2);
    WorkerStatus answ = srvWrk->checkQueue(caller3);
    ASSERT_EQ(WorkerStatus::OVERLOAD, answ);
}

TEST_F(ServerWorkerTest, ServerWorkerCallOk) {
    Caller caller(1234567890, 1);
    caller.setTimeoutDTSecs(5);
    WorkerStatus answ = srvWrk->checkQueue(caller);
    ASSERT_EQ(WorkerStatus::OK, answ);
}

TEST_F(ServerWorkerTest, ServerWorkerDuplication) {
    Caller caller(1234567890, 1);
    caller.setTimeoutDTSecs(5);
    srvWrk->checkQueue(caller);
    WorkerStatus answ = srvWrk->checkQueue(caller);
    ASSERT_EQ(WorkerStatus::DUPLICATE, answ);
}

