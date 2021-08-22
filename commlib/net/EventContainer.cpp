

#include "EventContainer.h"
#include "MacroAssemble.h"
#include "BaseJob.h"

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>

namespace CWSLib {
	EventContainer::EventContainer()
	{
		maxEvent = 100;
		timeout = 200;
	}

	int32_t EventContainer::Init(int32_t listenFd)
	{
		m_listenFd = listenFd;
		m_epfd = epoll_create(256);
		epoll_event initEvent;
		initEvent.data.fd = listenFd;
		initEvent.events = EPOLLIN | EPOLLET;
		epoll_ctl(m_epfd, EPOLL_CTL_ADD, listenFd, &initEvent);
		for (int i = 0; i < sizeof(events) / sizeof(events[0]); ++i)
		{
			bzero(&events[i], sizeof(events[0]));
		}
		return m_epfd;
	}

	int32_t EventContainer::Wait()
	{
		int fdNum = epoll_wait(m_epfd, events, maxEvent, timeout);
		for (int i = 0; i < fdNum; ++i)
		{
			if (events[i].data.fd == m_listenFd)
			{
				for (; ; )
				{
					std::shared_ptr<Socket> sock = m_listenFunc();
					if (!sock.get())
					{
						break;
					}
					int connFd = sock->GetFd();
					std::shared_ptr<SockCtx> ctx = std::make_shared<SockCtx>();
					ctx->available = true;
					ctx->sock = sock;
					m_sockMap.insert(std::make_pair(connFd, ctx));
					epoll_event registEvent;
					registEvent.data.fd = connFd;
					registEvent.events = EPOLLIN | EPOLLET;
					if (-1 == epoll_ctl(m_epfd, EPOLL_CTL_ADD, connFd, &registEvent))
					{
						ERROR_LOG("epoll_ctl");
						return -1;
					}
				}
			}
			else if (events[i].events & EPOLLIN)
			{
				auto itor = m_sockMap.find(events[i].data.fd);
				if (itor == m_sockMap.end())
				{
					ERROR_LOG("fd [%d] not cached.", events[i].data.fd);
					return -1;
				}
				if (itor->second->available == false)
				{
					continue;
				}
				itor->second->available = false;
				//int ret = m_readFunc(itor->second);
				auto job = CJobFactory::instance()->create();
				job->Init(itor->second->sock);
				int ret = job->ReadSock();
				if (ret != 0)
				{
					itor->second->sock->Close();
					m_sockMap.erase(events[i].data.fd);
					events[i].data.fd = -1;
					continue;
				}
				epoll_event registEvent;
				registEvent.data.fd = events[i].data.fd;
				registEvent.events = EPOLLOUT | EPOLLET;
				epoll_ctl(m_epfd, EPOLL_CTL_MOD, events[i].data.fd, &registEvent);
				job->Execute();
				itor->second->available = true;
			}
			else if (events[i].events & EPOLLOUT)
			{
				int sockFd = events[i].data.fd;
				epoll_event registEvent;
				registEvent.data.fd = sockFd;
				registEvent.events = EPOLLIN | EPOLLET;
				epoll_ctl(m_epfd, EPOLL_CTL_MOD, sockFd, &registEvent);
			}
		}

		return 0;
	}

	void EventContainer::OnListen(std::function<std::shared_ptr<Socket>()> func)
	{
		m_listenFunc = func;
	}

}
