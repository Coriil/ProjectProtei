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
    srvWrk->checkQueue(1234567890, 1);
    srvWrk->checkQueue(7894561230, 2);
    WorkerStatus answ = srvWrk->checkQueue(4561237890, 3);
    ASSERT_EQ(WorkerStatus::OVERLOAD, answ);
}

TEST_F(ServerWorkerTest, ServerWorkerCallOk) {
    WorkerStatus answ = srvWrk->checkQueue(1234567890, 1);
    ASSERT_EQ(WorkerStatus::OK, answ);
}

TEST_F(ServerWorkerTest, ServerWorkerDuplication) {
    srvWrk->checkQueue(1234567890, 1);
    WorkerStatus answ = srvWrk->checkQueue(1234567890, 1);
    ASSERT_EQ(WorkerStatus::DUPLICATE, answ);
}
