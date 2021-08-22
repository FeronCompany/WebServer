

#include <google/protobuf/message.h>

#include "CommonLib.h"

#include "user_service.pb.h"
#include "CwsChannel.h"

namespace CwsFrame
{
	void Channel::CallMethod(const google::protobuf::MethodDescriptor* method,
		google::protobuf::RpcController* controller,
		const google::protobuf::Message* request,
		google::protobuf::Message* response,
		google::protobuf::Closure* done)
	{
		//std::string requestData = request->SerializeAsString();
		std::string requestData;
		request->SerializeToString(&requestData);
		cws::common::RpcContext context;
		context.set_service_name(method->service()->name());
		context.set_method_name(method->name());
		context.set_content_length(requestData.length());
		//std::string ctxString = context.SerializeAsString();
		std::string ctxString;
		context.SerializeToString(&ctxString);
		std::string content;
		int32_t length = ctxString.length();
		NORMAL_LOG("context length: %d", length);
		content.insert(0, (char*)&length, sizeof(int32_t));
		content += ctxString;
		content += requestData;
		int32_t sendCnt = m_sock.WriteAll(content);
		NORMAL_LOG("Sent: %d", sendCnt);

		char buf[sizeof(int32_t)] = {0};
		m_sock.Read(buf, sizeof(int32_t));
		int32_t rLen = *(int32_t*)buf;
		NORMAL_LOG("Size[%d]", rLen);
		std::string respStr;
		m_sock.Read(respStr, rLen);
		response->ParseFromString(respStr);
	}

	void Channel::Init(const std::string& host, int32_t port)
	{
		m_sock.Init();
		int32_t ret = m_sock.Connect(host, port);
		NORMAL_LOG("connect res[%d]", ret);
	}

}


