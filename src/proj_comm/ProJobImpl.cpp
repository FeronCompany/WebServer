

#include <thread>

#include "CommonLib.h"

#include "CwsServer.h"

#include "ProJobImpl.h"
#include "ProError.h"

void ProJobImpl::Init(std::shared_ptr<CWSLib::Socket> sock)
{
	m_sock = sock;
}

int32_t ProJobImpl::ReadSock()
{
	char buf[sizeof(int32_t)] = { 0 };
	int readLen = m_sock->Read(buf, sizeof(int32_t));
	if (readLen <= 0)
	{
		return -1;
	}
	int32_t contextLen = *(int32_t*)buf;
	NORMAL_LOG("Context len[%d][%u]", contextLen, std::this_thread::get_id());
	std::string contextBuf;
	readLen = m_sock->Read(contextBuf, contextLen);
	NORMAL_LOG("Read from socket[%d][%u]", readLen, std::this_thread::get_id());
	if (readLen <= 0)
	{
		return -1;
	}
	m_context.ParseFromString(contextBuf);
	NORMAL_LOG("Service[%s], Method[%s].", m_context.service_name().c_str(), m_context.method_name().c_str());
	return 0;
}

int32_t ProJobImpl::Execute()
{
	try
	{
		CallMethod();
		Response();
	}
	catch (CWSLib::Exception& exp)
	{
		ERROR_LOG("Caught exception[%d][%s].", exp.error(), exp.what());
		return -1;
	}
	catch (std::exception& exp)
	{
		ERROR_LOG("Caught exception[%s].", exp.what());
		return -1;
	}
	catch (...)
	{
		ERROR_LOG("Caught unknown error.");
		return -1;
	}
	return 0;
}

void ProJobImpl::CallMethod()
{
	CwsFrame::Server& server = *CServerSingleton::instance();
	std::shared_ptr<CwsFrame::Service> service = server.GetService(m_context.service_name());
	if (!service.get())
	{
		throw CWSLib::Exception(RetCode::UNKNOWN_SERVICE, "Unknown service.");
	}
	m_method = service->GetMethod(m_context.method_name());
	if (!m_method.get())
	{
		throw CWSLib::Exception(RetCode::UNKNOWN_METHOD, "Unknown method.");
	}
	std::string messageBuf;
	m_sock->Read(messageBuf, m_context.content_length());
	auto request = m_method->GetRequest();
	request->ParseFromString(messageBuf);
	m_method->Execute();
}

void ProJobImpl::Response()
{
	auto response = m_method->GetResponse();
	std::string output;
	std::string respStr = response->SerializeAsString();
	size_t length = respStr.length();
	output.insert(0, (char*)&length, sizeof(int32_t));
	output += respStr;
	m_sock->WriteAll(output);
}

//REG_TYPE(CwsFrame::JobImpl, ProJobImpl, pro_job_impl, true);
