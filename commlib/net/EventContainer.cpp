

#include "EventContainer.h"
#include "commlib/app/MacroAssemble.h"

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

namespace CWSLib {
	EventContainer::EventContainer()
	{
	}

	int32_t EventContainer::Init(int32_t listenFd)
	{
		m_listenFd = listenFd;
		m_epfd = epoll_create(256);
		epoll_event initEvent;
		initEvent.data.fd = listenFd;
		initEvent.events = EPOLLIN | EPOLLET; // ����Ҫ������¼�����
		epoll_ctl(m_epfd, EPOLL_CTL_ADD, listenFd, &initEvent); // ע��epoll�¼�
		return m_epfd;
	}

	int32_t EventContainer::Wait()
	{
		// �ȴ�epoll�¼��ķ���
		int fdNum = epoll_wait(m_epfd, events, maxEvent, timeout);
		// �����������������¼�
		for (int i = 0; i < fdNum; ++i)
		{
			if (events[i].data.fd == m_listenFd)
			{
				// ���ڲ����˱�Ե����ģʽ��������Ҫʹ��ѭ������֤�����µ����Ӷ���ע��
				for (; ; )
				{
					int connFd = m_listenFunc();
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
				//������Ѿ����ӵ��û��������յ����ݣ���ô���ж��롣
				int ret = m_readFunc(events[i].data.fd);
				if (ret < 0)
				{
					DEBUG_LOG("Read from [%d] failed.", events[i].data.fd);
					events[i].data.fd = -1;
				}
				epoll_event registEvent;
				registEvent.data.fd = events[i].data.fd; // ��������д�������ļ�������
				registEvent.events = EPOLLOUT | EPOLLET; // ��������ע���д�����¼�
				epoll_ctl(m_epfd, EPOLL_CTL_MOD, events[i].data.fd, &registEvent); // �޸�sockFd��Ҫ������¼�ΪEPOLLOUT
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

	void EventContainer::OnListen(std::function<int32_t()> func)
	{
		m_listenFunc = func;
	}

	void EventContainer::OnRead(std::function<int32_t(int32_t)> func)
	{
		m_readFunc = func;
	}

}
