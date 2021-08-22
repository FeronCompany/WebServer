

#ifndef __PRO_JOB_IMPL_H__
#define __PRO_JOB_IMPL_H__

#include "BaseJob.h"

#include "common_define.pb.h"
#include "user_service.pb.h"

class ProJobImpl : public CWSLib::BaseJob
{
public:
	ProJobImpl() {}
	~ProJobImpl() {}

	void Init(std::shared_ptr<CWSLib::Socket> sock) override;
	int32_t ReadSock() override;
	int32_t Execute() override;

private:
	void CallMethod();
	void Response();

private:
	std::shared_ptr<CWSLib::Socket> m_sock;
	cws::common::RpcContext m_context;
	std::shared_ptr<CwsFrame::Method> m_method;
};

#endif // !__PRO_JOB_IMPL_H__

