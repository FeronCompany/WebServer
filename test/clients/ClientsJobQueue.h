

#ifndef __CLIENTS_JOB_QUEUE_H__
#define __CLIENTS_JOB_QUEUE_H__

#include <vector>
#include <memory>

#include "commlib/basic/CommonSingleton.h"

namespace CWSTest
{
	class ClientJobBase
	{
	public:
		virtual ~ClientJobBase() {}
		virtual int32_t Execute() = 0;
	};

	struct ExecResult
	{
		int32_t succ_num;
		int32_t fail_num;
		ExecResult() : succ_num(0), fail_num(0) {}
	};

	class ClientQueue
	{
	public:
		void Add(std::shared_ptr<ClientJobBase> job);
		void Exec();
		ExecResult& Result();

	private:
		std::vector<std::shared_ptr<ClientJobBase>> m_queue;
		ExecResult m_result;
	};
    using SClientQueue = CWSLib::CommSingleton<ClientQueue>;

	template<typename ClientImpl>
	class RegClientMata
	{
	public:
		RegClientMata()
		{
            SClientQueue::instance()->Add(std::shared_ptr<ClientJobBase>(new ClientImpl));
		}
	};
}

#define REG_CLIENT_TASK(TYPE) static CWSTest::RegClientMata<TYPE> TYPE##obj

#endif // !__CLIENTS_JOB_QUEUE_H__

