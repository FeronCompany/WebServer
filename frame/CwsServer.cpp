

#include "commlib/app/MacroAssemble.h"
#include "commlib/thread_pool/ThreadPool.h"

#include "CwsServer.h"
#include "CwsJobImpl.h"

namespace CwsFrame
{
    int32_t Server::AddService(const std::string& serviceName, Service* servicePtr)
    {
        if (m_serviceMap.find(serviceName) != m_serviceMap.end())
        {
            NORMAL_LOG("Adding service[%s]", serviceName.c_str());
            m_serviceMap.insert(std::make_pair(serviceName, std::shared_ptr<Service>(servicePtr)));
            return 0;
        }
        else
        {
            ERROR_LOG("Init server failed--duplicated service name[%s].", serviceName.c_str());
            return -1;
        }
    }

    std::shared_ptr<Service> Server::GetService(const std::string serviceName)
    {
        auto itor = m_serviceMap.find(serviceName);
        if (itor == m_serviceMap.end())
        {
            std::shared_ptr<Service> service = std::shared_ptr<Service>(new Service(serviceName));
            m_serviceMap.insert(std::make_pair(serviceName, service));
            return service;
        }
        else
        {
            return itor->second;
        }
    }

    void Server::Run()
    {
        CWSLib::ThreadPool& thdPool = CWSLib::CommSingleton<CWSLib::ThreadPool>::instance();
        thdPool.init(5, 100, 20);
        dispatcher.init([&](std::shared_ptr<CWSLib::Socket> sock) {
            auto job = CJobFactory::instance().create();
            job->Init(sock);
            thdPool.addTask(job);
        });
        NORMAL_LOG("Finish init server");
        dispatcher.wait();
    }

}