

#ifndef __USER_INFO_REGIST_H__
#define __USER_INFO_REGIST_H__

#include "CwsService.h"
#include "CwsServer.h"

#include "user_service.pb.h"

class UserInfoRegist : public CwsFrame::Method
{
public:
	UserInfoRegist();
	~UserInfoRegist();

	int32_t Execute() override;
	std::shared_ptr<google::protobuf::Message> GetRequest() override;
	std::shared_ptr<google::protobuf::Message> GetResponse() override;

private:

private:
	std::shared_ptr<cws::user::user_info_regist_rqst> m_request;
	std::shared_ptr<cws::user::user_info_regist_resp> m_response;
};

#endif // !__USER_INFO_REGIST_H__

