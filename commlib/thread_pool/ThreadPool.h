

#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <thread>
#include <list>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "BaseJob.h"
#include "CommonSingleton.h"
namespace CWSLib
{
	class JobList
	{
	public:
		~JobList();

		void addTask(BaseJob* task);
		BaseJob* getTask();
		void popTask();
		bool empty();
		int size();

	private:
		//std::list<BaseTask*> mTaskList;
		std::list<std::unique_ptr<BaseJob>> mTaskList;
	};

	class ThreadPool
	{
	public:
		ThreadPool() : mTaskLimit(0), mThreadLimit(0), mInitThdnum(0), mContinue(true),
			mCurrentThdSize(0) {}
		~ThreadPool();

		// initThreadNum -- number of working threads
		// maxTaskNum -- max task number in task list
		// maxThreadNum -- max working thread number
		void init(int initThreadNum, int maxTaskNum, int maxThreadNum);
		void addTask(BaseJob* task);
		void shutdown();

	private:
		void work();

	private:
		int mTaskLimit;
		int mThreadLimit;
		int mInitThdnum;
		int mCurrentThdSize;
		bool mContinue;

		JobList mJobList;
		std::mutex mMutex;
		std::condition_variable mWakeCond;
		std::condition_variable mPushCond;
	};
    using SThreadPool = CommSingleton<ThreadPool>;
}

#endif // !__THREAD_POOL_H__

