

#include "EventDispatcher.h"
#include "commlib/app/MacroAssemble.h"

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

namespace CWSLib
{
	const static int LISTENQ = 20;

	EventDispatcher::EventDispatcher() :
		m_epSock(epoll_create(256))
	{
		timeout = 200;
		maxEvent = 100;
	}

	EventDispatcher::~EventDispatcher()
	{
	}

	int32_t EventDispatcher::init(int port, std::function<void(CWSLib::Socket&, const std::string&)> cbFunc)
	{
		m_listenSock.SetNonblocking();

		epoll_event initEvent;
		initEvent.data.fd = m_listenSock.GetFd();
		initEvent.events = EPOLLIN | EPOLLET; // ����Ҫ������¼�����
		epoll_ctl(m_epSock.GetFd(), EPOLL_CTL_ADD, m_listenSock.GetFd(), &initEvent); // ע��epoll�¼�

		sockaddr_in serverAddr;
		bzero(&serverAddr, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		std::string localAddr = "127.0.0.1";
		inet_aton(localAddr.c_str(), &(serverAddr.sin_addr));

		//int portNumber = SERV_PORT;
		serverAddr.sin_port = htons(port);
		bind(m_listenSock.GetFd(), (sockaddr*)&serverAddr, sizeof(serverAddr));
		listen(m_listenSock.GetFd(), LISTENQ);

		mCb = cbFunc;
		return 0;
	}

	int32_t EventDispatcher::tick()
	{
		char hostBuf[NI_MAXHOST]; // IP��ַ����
		char portBuf[NI_MAXSERV]; // PORT����
		epoll_event events[20];

		// �ȴ�epoll�¼��ķ���
		int fdNum = epoll_wait(m_epSock.GetFd(), events, maxEvent, timeout);
		// �����������������¼�
		for (int i = 0; i < fdNum; ++i)
		{
			if (events[i].data.fd == m_listenSock.GetFd())
			{
				// ���ڲ����˱�Ե����ģʽ��������Ҫʹ��ѭ������֤�����µ����Ӷ���ע��
				for (; ; )
				{
					sockaddr_in clientAddr = { 0 };
					socklen_t addrSize = sizeof(clientAddr);
					int connFd = accept(m_listenSock.GetFd(), (sockaddr*)&clientAddr, &addrSize);
					if (-1 == connFd)
					{
						break;
					}
					Socket newConn(connFd);
					int ret = getnameinfo((sockaddr*)&clientAddr, sizeof(clientAddr),
						hostBuf, sizeof(hostBuf) / sizeof(hostBuf[0]),
						portBuf, sizeof(portBuf) / sizeof(portBuf[0]),
						NI_NUMERICHOST | NI_NUMERICSERV);
					if (!ret)
					{
						DEBUG_LOG("New connection: host = %s, port = %s\n", hostBuf, portBuf);
					}
					DEBUG_LOG("Receive new connection[%d]\n", connFd);

					newConn.SetNonblocking();
					epoll_event registEvent;
					registEvent.data.fd = connFd; // �������ڶ��������ļ�������
					registEvent.events = EPOLLIN | EPOLLET; // ��������ע��Ķ������¼�
					// Ϊ��accept�� file describe ����epoll�¼�
					if (-1 == epoll_ctl(m_epSock.GetFd(), EPOLL_CTL_ADD, connFd, &registEvent))
					{
						ERROR_LOG("epoll_ctl");
						return -1;
					}
				}
			}
			else if (events[i].events & EPOLLIN)
			{
				std::string out;
				//int sockFd;
				//������Ѿ����ӵ��û��������յ����ݣ���ô���ж��롣
				DEBUG_LOG("EPOLLIN");
				Socket sock(events[i].data.fd);
				if (sock.GetFd() < 0)
					continue;
				ssize_t socketLen;
				if ((socketLen = sock.ReadAll(out)) < 0)
				{
					if (errno == ECONNRESET)
					{
						sock.Close();
						events[i].data.fd = -1;
						//epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
						DEBUG_LOG("Close socket[%d]\n", sock.GetFd());
					}
					else
					{
						DEBUG_LOG("read out");
					}
				}
				else if (socketLen == 0)
				{
					sock.Close();
					events[i].data.fd = -1;
					//epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
					DEBUG_LOG("Close socket[%d]\n", sock.GetFd());
				}
				DEBUG_LOG("Read line[%s]\n", out.c_str());
				epoll_event registEvent;
				registEvent.data.fd = sock.GetFd(); // ��������д�������ļ�������
				registEvent.events = EPOLLOUT | EPOLLET; // ��������ע���д�����¼�
				epoll_ctl(m_epSock.GetFd(), EPOLL_CTL_MOD, sock.GetFd(), &registEvent); // �޸�sockFd��Ҫ������¼�ΪEPOLLOUT
				mCb(sock, out);
			}
			else if (events[i].events & EPOLLOUT) // ��������ݷ���
			{
				//char line[MAXLINE];
				int sockFd = events[i].data.fd;
				//writeAll(sockFd, line, strnlen(line, sizeof(line)));
				epoll_event registEvent;
				registEvent.data.fd = sockFd; // �������ڶ��������ļ�������
				registEvent.events = EPOLLIN | EPOLLET; // ��������ע��Ķ������¼�
				epoll_ctl(m_epSock.GetFd(), EPOLL_CTL_MOD, sockFd, &registEvent); // �޸�sockFd��Ҫ������¼�ΪEPOLIN
			}
		}

		return 0;
	}
}

