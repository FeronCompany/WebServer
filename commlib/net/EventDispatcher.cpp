

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

#include "EventDispatcher.h"
#include "MacroAssemble.h"

namespace CWSLib
{
	EventDispatcher::EventDispatcher()
	{
	}

	EventDispatcher::~EventDispatcher()
	{
	}

	int32_t EventDispatcher::init()
	{
		acceptor.Init();
		acceptor.SetNonblocking();
		acceptor.Bind("127.0.0.1", 9091);
		container.Init(acceptor.GetFd());
		container.OnListen([&]() {
			std::shared_ptr<Socket> sock = acceptor.Accept();
			if(sock.get())
				sock->SetNonblocking();
			return sock;
		});
		return 0;
	}

	int32_t EventDispatcher::wait()
	{
		while (true)
		{
			if (container.Wait())
			{
				ERROR_LOG("Container wait error.");
				break;
			}
		}
		return 0;
	}
}

