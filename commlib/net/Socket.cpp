#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include "Socket.h"
#include "commlib/app/MacroAssemble.h"

namespace CWSLib
{
	const static int LISTENQ = 20;

	Socket::Socket()
	{
		
	}

	Socket::Socket(int32_t fd)
	{
		Init(fd);
	}

	Socket::~Socket()
	{
		Close();
	}

	void Socket::Init(int32_t fd)
	{
		m_fd = fd;
	}

	void Socket::Init()
	{
		m_fd = socket(AF_INET, SOCK_STREAM, 0);
	}

	int32_t Socket::GetFd()
	{
		return m_fd;
	}

	int32_t Socket::GetPort()
	{
		return m_port;
	}

	std::string Socket::GetHost()
	{
		return m_host;
	}

	void Socket::Close()
	{
		if (m_fd != 0)
		{
			NORMAL_LOG("Closing socket[%s:%d]", m_host.c_str(), m_port);
			close(m_fd);
		}
	}

	int32_t Socket::SetNonblocking()
	{
		int opts;
		opts = fcntl(m_fd, F_GETFL);
		if (opts < 0)
		{
			return -1;
		}
		opts = opts | O_NONBLOCK;
		if (fcntl(m_fd, F_SETFL, opts) < 0)
		{
			return -2;
		}
		return 0;
	}

	int32_t Socket::Connect(const std::string& host, int32_t port)
	{
		struct sockaddr_in servaddr;
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		inet_pton(AF_INET, host.c_str(), &servaddr.sin_addr);
		servaddr.sin_port = htons(port);
		m_host = host;
		m_port = port;
		return connect(m_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	}

	int32_t Socket::ReadAll(std::string& out)
	{
		char buf[MAX_BUF_SIZE] = { 0 };
		int32_t recvSum = 0;
		int32_t recvCnt = recv(m_fd, buf, MAX_BUF_SIZE, MSG_DONTWAIT | MSG_PEEK);
		while (true)
		{
			int32_t readLen = read(m_fd, buf + recvSum, MAX_BUF_SIZE - recvSum);
			if (readLen < 0)
			{
				return -1;
			}
			else if (readLen == 0)
			{
				break;
			}
			
			recvSum += readLen;
			//write(sock, buf, str_len);
			if (recvSum >= recvCnt)
				break;
		}
		out.append(buf);
		return recvSum;
	}

	int32_t Socket::Read(std::string& out, size_t length)
	{
		char* buf = new char[length + 1];
		int32_t readLen = read(m_fd, buf, length);
		out = buf;
		delete[] buf;
		return readLen;
	}

	int32_t Socket::Read(char* buf, size_t length)
	{
		return read(m_fd, buf, length);
	}

	int32_t Socket::WriteAll(const std::string& content)
	{
		int32_t sendCnt = 0;
		size_t size = content.length();
		while (true)
		{
			int32_t senSize = write(m_fd, content.c_str() + sendCnt, size - sendCnt);
			if (senSize < 0)
			{
				return -1;
			}
			else if (senSize == 0)
			{
				break;
			}
			sendCnt += senSize;
		}
		//write(sock, buf, size);
		return sendCnt;
	}

	void Socket::Bind(const std::string host, int32_t port)
	{
		sockaddr_in serverAddr;
		bzero(&serverAddr, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		inet_aton(host.c_str(), &(serverAddr.sin_addr));

		serverAddr.sin_port = htons(port);
		bind(m_fd, (sockaddr*)&serverAddr, sizeof(serverAddr));
		listen(m_fd, LISTENQ);
	}

	std::shared_ptr<Socket> Socket::Accept()
	{
		sockaddr_in clientAddr = { 0 };
		socklen_t addrSize = sizeof(clientAddr);
		int connFd = accept(m_fd, (sockaddr*)&clientAddr, &addrSize);
		if (-1 == connFd)
		{
			return std::shared_ptr<Socket>();
		}

		char hostBuf[NI_MAXHOST]; // IPµÿ÷∑ª∫¥Ê
		char portBuf[NI_MAXSERV]; // PORTª∫¥Ê
		int ret = getnameinfo((sockaddr*)&clientAddr, sizeof(clientAddr),
			hostBuf, sizeof(hostBuf) / sizeof(hostBuf[0]),
			portBuf, sizeof(portBuf) / sizeof(portBuf[0]),
			NI_NUMERICHOST | NI_NUMERICSERV);
		if (!ret)
		{
			DEBUG_LOG("New connection: host = %s, port = %s", hostBuf, portBuf);
		}
		std::shared_ptr<Socket> newConn = std::shared_ptr<Socket>(new Socket(connFd));
		newConn->SetNonblocking();
		newConn->m_host = hostBuf;
		newConn->m_port = std::atoi(portBuf);
		return newConn;
	}

}
