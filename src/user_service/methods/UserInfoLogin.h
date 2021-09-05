

#ifndef __USER_INFO_LOGIN_H__
#define __USER_INFO_LOGIN_H__

#include "CwsService.h"
#include "CwsServer.h"

#include "user_service.pb.h"

class UserInfoLogin : public CwsFrame::Method
{
public:
    UserInfoLogin();
    ~UserInfoLogin();

    int32_t Execute() override;
    std::shared_ptr<google::protobuf::Message> GetRequest() override;
    std::shared_ptr<google::protobuf::Message> GetResponse() override;

private:

private:
    std::shared_ptr<cws::user::user_info_login_rqst> m_request;
    std::shared_ptr<cws::user::user_info_login_resp> m_response;
};

#endif // !__USER_INFO_LOGIN_H__

