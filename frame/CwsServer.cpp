

#include "CwsServer.h"

namespace CwsFrame
{
    int32_t Server::AddService(const std::string& serviceName, Service* servicePtr)
    {
        if (m_serviceMap.find(serviceName) == m_serviceMap.end())
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
        NORMAL_LOG("[%u] services has been registed.", m_serviceMap.size());
        CWSLib::ThreadPool& thdPool = *CWSLib::SThreadPool::instance();
        thdPool.init(1, 100, 1);
        dispatcher.init();
        NORMAL_LOG("Finish init server");
        dispatcher.wait();
    }

}
