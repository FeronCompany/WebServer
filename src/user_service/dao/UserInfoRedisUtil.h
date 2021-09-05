

#pragma once

#include <string>

#include "user.pb.h"
#include "ProError.h"

class UserInfoRedisUtil
{
public:
    static RetCode UserSessionGet(const std::string& session_key, cws::user::UserSessionInfo& session);

    static RetCode UserSessionSet(const std::string& session_key, const cws::user::UserSessionInfo& session);

    static RetCode SessionKeyGet(const std::string& user_id, std::string& session_key);

    static RetCode SessionKeySet(const std::string& user_id, const std::string& session_key);

private:
    
};
