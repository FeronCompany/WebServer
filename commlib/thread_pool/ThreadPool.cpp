#include "ThreadPool.h"
#include <iostream>

CWSLib::ThreadPool::~ThreadPool()
{
}

void CWSLib::ThreadPool::init(int initThreadNum, int maxTaskNum, int maxThreadNum)
{
	mInitThdnum = initThreadNum;
	mThreadLimit = maxThreadNum;
	mTaskLimit = maxTaskNum;

	// 初始化工作线程
	for (int i = 0; i < initThreadNum; ++i)
	{
		std::thread thd([=]() {
			this->work();
			});
		thd.detach();
	}
}

void CWSLib::ThreadPool::addTask(BaseJob* task)
{
	if (!task)
	{
		return;
	}

	// 看任务队列是否已达阈值以及线程是否已达上限
	std::unique_lock<std::mutex> ulock(mMutex);
	if (mJobList.size() < mTaskLimit)
	{
		mJobList.addTask(task);
		mWakeCond.notify_one();
	}
	else if (mCurrentThdSize < mThreadLimit)
	{
		mJobList.addTask(task);
		mWakeCond.notify_one();
		std::thread thd([&]() {
			this->work();
			});
		thd.detach();
	}
	else
	{
		mPushCond.wait(ulock, [&]() {
			return (mJobList.size() < mTaskLimit) || (mCurrentThdSize < mThreadLimit);
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
		std::unique_ptr<BaseJob> task;

		{
			std::unique_lock<std::mutex> ulock(mMutex);
			mWakeCond.wait(ulock, [&]() {
				return !this->mContinue || !this->mJobList.empty();
				});
			if (!this->mContinue)
			{
				//std::cout << std::this_thread::get_id() << " quit\n";
				mCurrentThdSize--;
				//std::cout << "thread size:" << mCurrentThdSize << "\n";
				return;
			}

			task = std::unique_ptr<BaseJob>(mJobList.getTask());
			mJobList.popTask();

		}

		// 当任务队列减少时，唤醒任务队列
		if (mJobList.size() < mTaskLimit)
		{
			mPushCond.notify_one();
		}
		task->Execute();

		// 根据实时的任务情况，适当减少当前运行的线程
		if (mJobList.size() < mTaskLimit / 2 && mCurrentThdSize > mInitThdnum)
		{
			std::unique_lock<std::mutex> ulock(mMutex);
			//std::cout << std::this_thread::get_id() << " quit\n";
			mCurrentThdSize--;
			//std::cout << "thread size:" << mCurrentThdSize << "\n";
			return;
		}
	}
}




CWSLib::JobList::~JobList()
{
}

void CWSLib::JobList::addTask(BaseJob* task)
{
	mTaskList.push_back(std::unique_ptr<BaseJob>(task));
}

CWSLib::BaseJob* CWSLib::JobList::getTask()
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

void CWSLib::JobList::popTask()
{
	if (!mTaskList.empty())
	{
		mTaskList.pop_front();
	}
}

bool CWSLib::JobList::empty()
{
	return mTaskList.empty();
}

int CWSLib::JobList::size()
{
	return mTaskList.size();
}
