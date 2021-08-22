

#ifndef __CWS_CHANNEL_H__
#define __CWS_CHANNEL_H__

#include <google/protobuf/service.h>

#include "Socket.h"

namespace CwsFrame
{
	class Channel : public google::protobuf::RpcChannel
	{
	public:
		void CallMethod(const google::protobuf::MethodDescriptor* method,
			google::protobuf::RpcController* controller,
			const google::protobuf::Message* request,
			google::protobuf::Message* response,
			google::protobuf::Closure* done) override;

		void Init(const std::string& host, int32_t port);

	private:
		CWSLib::Socket m_sock;
	};
}

#endif // __CWS_CHANNEL_H__
