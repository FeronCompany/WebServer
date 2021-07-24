

#ifndef __TEST_TASK_H__
#define __TEST_TASK_H__

#include <chrono>
#include <iostream>
#include <thread>
#include <ctime>
#include <cstdlib>

#include "commlib/net/BaseJob.h"

class TestTask : public CWSLib::BaseJob
{
public:
    static std::string name;
public:
    void Init(std::shared_ptr<CWSLib::Socket> sock) override
    {

    }

    int32_t ReadSock() override
    {
        return 0;
    }

    int32_t Execute() override
    {
        srand((unsigned int)time(NULL));
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 300));
        std::cout << std::this_thread::get_id() << ": Haha\n";
        return 0;
    }

private:

};
std::string TestTask::name("TestTask");
#endif // !__TEST_TASK_H__

