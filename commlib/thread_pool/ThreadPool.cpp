

#include "ThreadPool.h"

#include <iostream>

void CWSLib::ThreadPool::init(int initThreadNum, int maxTaskNum, int maxThreadNum)
{
    mInitThdnum = initThreadNum;
    mThreadLimit = maxThreadNum;
    mTaskLimit = maxTaskNum;

    // ��ʼ�������߳�
    for (int i = 0; i < initThreadNum; ++i)
    {
        std::thread thd([&]() {
            this->work();
            });
        thd.detach();
    }
}

void CWSLib::ThreadPool::addTask(BaseTask* task)
{
    if (!task)
    {
        return;
    }

    // ����������Ƿ��Ѵ���ֵ�Լ��߳��Ƿ��Ѵ�����
    std::unique_lock<std::mutex> ulock(mMutex);
    if (mTasks.size() < mTaskLimit)
    {
        mTasks.addTask(task);
        mWakeCond.notify_one();
    }
    else if (mCurrentThdSize < mThreadLimit)
    {
        std::thread thd([&]() {
            this->work();
            });
        thd.detach();
        mTasks.addTask(task);
        mWakeCond.notify_one();
    }
    else
    {
        mPushCond.wait(ulock, [&]() {
            return (mTasks.size() < mTaskLimit) || (mCurrentThdSize < mThreadLimit);
            });
    }
}

void CWSLib::ThreadPool::shutdown()
{
    mContinue = false;
}

void CWSLib::ThreadPool::work()
{
    {
        std::unique_lock<std::mutex> ulock(mMutex);
        mCurrentThdSize++;
    }

    while (true)
    {
        std::unique_ptr<BaseTask> task;

        {
            std::unique_lock<std::mutex> ulock(mMutex);
            mWakeCond.wait(ulock, [&]() {
                return !this->mContinue || !this->mTasks.empty();
                });
            if (!this->mContinue)
            {
                return;
            }

            task = std::unique_ptr<BaseTask>(mTasks.getTask());
            mTasks.popTask();

        }

        // ��������м���ʱ�������������
        if (mTasks.size() < mTaskLimit)
        {
            mPushCond.notify_one();
        }
        task->excute();

        // ����ʵʱ������������ʵ����ٵ�ǰ���е��߳�
        if (mTasks.size() < mTaskLimit / 2 && mCurrentThdSize > mInitThdnum)
        {
            std::unique_lock<std::mutex> ulock(mMutex);
            std::cout << std::this_thread::get_id() << " quit\n";
            mCurrentThdSize--;
            std::cout << "thread size:" << mCurrentThdSize << "\n";
            return;
        }
    }
}




CWSLib::TaskList::~TaskList()
{
}

void CWSLib::TaskList::addTask(BaseTask* task)
{
    mTaskList.push_back(std::unique_ptr<BaseTask>(task));
}

CWSLib::BaseTask* CWSLib::TaskList::getTask()
{
    if (!mTaskList.empty())
    {
        return mTaskList.front().release();
    }
    else
    {
        return nullptr;
    }
}

void CWSLib::TaskList::popTask()
{
    if (!mTaskList.empty())
    {
        mTaskList.pop_front();
    }
}

bool CWSLib::TaskList::empty()
{
    return mTaskList.empty();
}

int CWSLib::TaskList::size()
{
    return mTaskList.size();
}