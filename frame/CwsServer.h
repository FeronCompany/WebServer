

#ifndef __CWS_SERVER_H__
#define __CWS_SERVER_H__

#include <string>
#include <map>
#include <memory>

#include "CommonLib.h"
#include "CwsService.h"

namespace CwsFrame
{
	class Server
	{
	public:
		int32_t AddService(const std::string& serviceName, Service* servicePtr);
		std::shared_ptr<Service> GetService(const std::string serviceName);
		void Run();

	private:
		std::map<std::string, std::shared_ptr<Service>> m_serviceMap;
		CWSLib::EventDispatcher dispatcher;
	};
	template<typename TMethod>
	class MethodRegMeta
	{
	public:
		MethodRegMeta(const std::string& serviceName, const std::string& methodName)
		{
			CWSLib::CommSingleton<CwsFrame::Server>::instance()->GetService(serviceName)->AddMethod(methodName, Instance);
		}

		static std::shared_ptr<TMethod> Instance()
		{
			return std::shared_ptr<TMethod>(new TMethod);
		}
	};
}

using CServerSingleton = CWSLib::CommSingleton<CwsFrame::Server>;

#define REG_SRV_METHOD(method, service_name, method_name) \
	static CwsFrame::MethodRegMeta<method> \
	meta_ ## service_name ## _ ## method_name(#service_name, #method_name);

#endif // !__CWS_SERVER_H__

