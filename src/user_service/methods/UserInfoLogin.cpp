

#include "user.pb.h"
#include "UserInfoDBUtil.h"
#include "UserInfoRedisUtil.h"
#include "UserInfoLogin.h"

UserInfoLogin::UserInfoLogin()
{
    m_request.reset(new cws::user::user_info_login_rqst);
    m_response.reset(new cws::user::user_info_login_resp);
}

UserInfoLogin::~UserInfoLogin()
{
    
}

int32_t UserInfoLogin::Execute()
{
    cws::user::UserInfo userInfo;
    auto err_code = UserInfoDBUtil::UserInfoGet(m_request->id(), userInfo);
    if (RetCode::SUCCESS != err_code)
    {
        ERROR_LOG("Get user info from DB failed.");
        m_response->mutable_ret_info()->set_err_code(RetCode::QUERY_USER_FAILED);
        m_response->mutable_ret_info()->set_err_msg("Query from DB failed.");
        return 0;
    }
    if (m_request->password() != userInfo.password())
    {
        ERROR_LOG(
            "Password of %s check failed [%s]/[%s].",
            m_request->id().c_str(),
            m_request->password().c_str(),
            userInfo.password().c_str());
        m_response->mutable_ret_info()->set_err_code(RetCode::PASSWORD_INVALID);
        m_response->mutable_ret_info()->set_err_msg("Password check failed.");
        return 0;
    }

    std::string session_key;
    err_code = UserInfoRedisUtil::SessionKeyGet(m_request->id(), session_key);
    if (RetCode::NOT_EXIST == err_code)
    {
        session_key = CWSLib::String::genRandomCode(12);
        UserInfoRedisUtil::SessionKeySet(m_request->id(), session_key);
        cws::user::UserSessionInfo session_info;
        session_info.set_user_id(m_request->id());
        UserInfoRedisUtil::UserSessionSet(session_key, session_info);
        m_response->set_session_key(session_key);
    }
    else if (RetCode::SUCCESS == err_code)
    {
        m_response->set_session_key(session_key);
    }
    else
    {
        ERROR_LOG("SessionKeyGet by {} failed.", m_request->id().c_str());
        m_response->mutable_ret_info()->set_err_code(RetCode::UNKNOWN_ERROR);
        m_response->mutable_ret_info()->set_err_msg("OK");
        return 0;
    }

    m_response->mutable_ret_info()->set_err_code(RetCode::SUCCESS);
    m_response->mutable_ret_info()->set_err_msg("OK");
    return 0;
}

std::shared_ptr<google::protobuf::Message> UserInfoLogin::GetRequest()
{
    return m_request;
}

std::shared_ptr<google::protobuf::Message> UserInfoLogin::GetResponse()
{
    return m_response;
}
