#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <CDRWorker.h>

using namespace testing;



TEST(CDRWorkerTest, CDRjournalRecordOk_recCallOverload)
{
   CDRWorker cdrWrk;
   QDateTime curDT = QDateTime::currentDateTime();
   int answ = cdrWrk.recCallOverload(curDT,1,7894561230);
   ASSERT_EQ(answ,0);
}

TEST(CDRWorkerTest, CDRjournalRecordOk_recCallODuplication)
{
   CDRWorker cdrWrk;
   QDateTime curDT = QDateTime::currentDateTime();
   int answ = cdrWrk.recCallDuplication(curDT,1,7894561230);
   ASSERT_EQ(answ,0);
}

TEST(CDRWorkerTest, CDRjournalRecordNotOk_recFinishAnsweredCall)
{
   CDRWorker cdrWrk;
   QDateTime curDT = QDateTime::currentDateTime();
   int answ = cdrWrk.recFinishAnsweredCall(curDT,1);
   ASSERT_NE(answ,0);
}

TEST(CDRWorkerTest, CDRjournalRecordOk_recFinishAnsweredCall)
{
   CDRWorker cdrWrk;
   QDateTime curDT = QDateTime::currentDateTime();
   cdrWrk.recInCall(curDT, 1, 7894561230);
   curDT = QDateTime::currentDateTime();
   cdrWrk.recAnswerCall(curDT, 2, 1);
   curDT = QDateTime::currentDateTime();
   int answ = cdrWrk.recFinishAnsweredCall(curDT, 1);
   ASSERT_EQ(answ,0);
}

TEST(CDRWorkerTest, CDRjournalRecordOk_TimeoutedCall)
{
    CDRWorker cdrWrk;
    QDateTime curDT = QDateTime::currentDateTime();
    cdrWrk.recInCall(curDT, 1, 7894561230);
    int answ = cdrWrk.recTimeoutedCall(1);
    ASSERT_EQ(answ,0);
}
