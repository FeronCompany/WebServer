

#include "EventContainer.h"
#include "commlib/app/MacroAssemble.h"

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
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
		initEvent.events = EPOLLIN | EPOLLET; // ����Ҫ������¼�����
		epoll_ctl(m_epfd, EPOLL_CTL_ADD, listenFd, &initEvent); // ע��epoll�¼�
		for (int i = 0; i < sizeof(events) / sizeof(events[0]); ++i)
		{
			bzero(&events[i], sizeof(events[0]));
		}
		return m_epfd;
	}

	int32_t EventContainer::Wait()
	{
		// �ȴ�epoll�¼��ķ���
		int fdNum = epoll_wait(m_epfd, events, maxEvent, timeout);
		// �����������������¼�
		for (int i = 0; i < fdNum; ++i)
		{
			NORMAL_LOG("==X==");
			if (events[i].data.fd == m_listenFd)
			{
				// ���ڲ����˱�Ե����ģʽ��������Ҫʹ��ѭ������֤�����µ����Ӷ���ע��
				for (; ; )
				{
					std::shared_ptr<Socket> sock = m_listenFunc();
					int connFd = sock->GetFd();
					m_sockMap.insert(std::make_pair(connFd, sock));
					if (connFd < 0)
					{
						ERROR_LOG("Accept new socket failed");
						return -1;
					}
					epoll_event registEvent;
					registEvent.data.fd = connFd; // �������ڶ��������ļ�������
					registEvent.events = EPOLLIN | EPOLLET; // ��������ע��Ķ������¼�
					// Ϊ��accept�� file describe ����epoll�¼�
					if (-1 == epoll_ctl(m_epfd, EPOLL_CTL_ADD, connFd, &registEvent))
					{
						ERROR_LOG("epoll_ctl");
						return -1;
					}
				}
			}
			else if (events[i].events & EPOLLIN)
			{
				NORMAL_LOG("==X==");
				// ������Ѿ����ӵ��û��������յ����ݣ���ô���ж��롣
				auto itor = m_sockMap.find(events[i].data.fd);
				if (itor == m_sockMap.end())
				{
					ERROR_LOG("fd [%d] not cached.", events[i].data.fd);
					return -1;
				}
				int ret = m_readFunc(itor->second);
				NORMAL_LOG("==X==");
				if (ret < 0)
				{
					DEBUG_LOG("Read from [%d] failed.", events[i].data.fd);
					events[i].data.fd = -1;
				}
				epoll_event registEvent;
				registEvent.data.fd = events[i].data.fd; // ��������д�������ļ�������
				registEvent.events = EPOLLOUT | EPOLLET; // ��������ע���д�����¼�
				epoll_ctl(m_epfd, EPOLL_CTL_MOD, events[i].data.fd, &registEvent); // �޸�sockFd��Ҫ������¼�ΪEPOLLOUT
				NORMAL_LOG("==X==");
			}
			else if (events[i].events & EPOLLOUT) // ��������ݷ���
			{
				int sockFd = events[i].data.fd;
				epoll_event registEvent;
				registEvent.data.fd = sockFd; // �������ڶ��������ļ�������
				registEvent.events = EPOLLIN | EPOLLET; // ��������ע��Ķ������¼�
				epoll_ctl(m_epfd, EPOLL_CTL_MOD, sockFd, &registEvent); // �޸�sockFd��Ҫ������¼�ΪEPOLIN
			}
		}

		return 0;
	}

	void EventContainer::OnListen(std::function<std::shared_ptr<Socket>()> func)
	{
		m_listenFunc = func;
	}

	void EventContainer::OnRead(std::function<int32_t(std::shared_ptr<Socket>)> func)
	{
		m_readFunc = func;
	}

}
