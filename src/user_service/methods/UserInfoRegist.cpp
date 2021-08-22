

#include "commlib/app/CommonLib.h"

#include "UserInfoRegist.h"
#include "UserInfoDBUtil.h"

UserInfoRegist::UserInfoRegist()
{
    m_request.reset(new cws::user::user_info_regist_rqst);
    m_response.reset(new cws::user::user_info_regist_resp);
}

UserInfoRegist::~UserInfoRegist()
{
}

int32_t UserInfoRegist::Execute()
{
    NORMAL_LOG("User name[%s], password[%s]", m_request->user_name().c_str(), m_request->password().c_str());
    std::string oid = CWSLib::String::genRandomCode(10);
    cws::user::UserInfo userInfo;
    userInfo.set_user_uid(CWSLib::String::genRandomCode(12));
    userInfo.set_oid(oid);
    userInfo.set_user_name(m_request->user_name());
    userInfo.set_phone_number(m_request->phone_number());
    userInfo.set_gender(m_request->gender());
    userInfo.set_birthday(m_request->birthday());
    userInfo.set_password(m_request->password());
    auto err_code = UserInfoDBUtil::UserInfoRegist(userInfo);
    if (RetCode::SUCCESS != err_code)
    {
        ERROR_LOG("UserInfoRegist failed.");
        m_response->mutable_ret_info()->set_err_code(RetCode::REGIST_USER_FAILED);
        m_response->mutable_ret_info()->set_err_msg("Regist failed.");
        return 0;
    }
    
    m_response->mutable_ret_info()->set_err_code(0);
    m_response->mutable_ret_info()->set_err_msg("OK");
    m_response->set_user_id(oid);
    return 0;
}

std::shared_ptr<google::protobuf::Message> UserInfoRegist::GetRequest()
{
    return m_request;
}

std::shared_ptr<google::protobuf::Message> UserInfoRegist::GetResponse()
{
    return m_response;
}

//REG_SRV_METHOD(UserInfoRegist, user_service, user_info_regist);

