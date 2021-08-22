

#ifndef __BASE_JOB_H__
#define __BASE_JOB_H__

#include <memory>

#include "TypeRegSingleton.h"
#include "Socket.h"

namespace CWSLib
{
	class BaseJob
	{
	public:
		virtual ~BaseJob() {};
		virtual void Init(std::shared_ptr<Socket> sock) = 0;
		virtual int32_t Execute() = 0;
		virtual int32_t ReadSock() = 0;
	};

}
using CJobFactory = CWSLib::CommSingleton<TypeFactory<CWSLib::BaseJob>>;

#endif // !__BASE_JOB_H__

