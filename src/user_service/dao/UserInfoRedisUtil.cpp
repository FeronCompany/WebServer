

#include <sw/redis++/redis++.h>

#include "CommonLib.h"

#include "UserInfoRedisUtil.h"

#define USER_SESSION_TABLE "user_session"
#define SESSION_KEY_TABLE "session_key"

RetCode UserInfoRedisUtil::UserSessionGet(const std::string& session_key, cws::user::UserSessionInfo& session)
{
    if (session_key.empty())
    {
        ERROR_LOG("user id is empty.");
        return RetCode::PARAM_ERROR;
    }

    auto instance = CWSLib::SingletonFactory<sw::redis::Redis>::Instance();
    auto o_string = instance->hget(USER_SESSION_TABLE, session_key);
    if (!o_string)
    {
        ERROR_LOG("hget user %s session failed", session_key.c_str());
        return RetCode::NOT_EXIST;
    }
    CWSLib::JsonStringToProto(*o_string, session);
    return RetCode::SUCCESS;
}

RetCode UserInfoRedisUtil::UserSessionSet(const std::string& session_key, const cws::user::UserSessionInfo& session)
{
    if (session_key.empty())
    {
        ERROR_LOG("user id is empty.");
        return RetCode::PARAM_ERROR;
    }

    std::string output;
    CWSLib::ProtoToJsonString(session, output);
    auto instance = CWSLib::SingletonFactory<sw::redis::Redis>::Instance();
    instance->hset(USER_SESSION_TABLE, session_key, output);
    return RetCode::SUCCESS;
}

RetCode UserInfoRedisUtil::SessionKeyGet(const std::string& user_id, std::string& session_key)
{
    if (user_id.empty())
    {
        ERROR_LOG("user id is empty.");
        return RetCode::PARAM_ERROR;
    }

    auto instance = CWSLib::SingletonFactory<sw::redis::Redis>::Instance();
    auto o_string = instance->hget(SESSION_KEY_TABLE, user_id);
    if (!o_string)
    {
        ERROR_LOG("hget user %s session failed", user_id.c_str());
        return RetCode::NOT_EXIST;
    }
    session_key = *o_string;
    return RetCode::SUCCESS;
}

RetCode UserInfoRedisUtil::SessionKeySet(const std::string& user_id, const std::string& session_key)
{
    if (user_id.empty())
    {
        ERROR_LOG("user id is empty.");
        return RetCode::PARAM_ERROR;
    }

    auto instance = CWSLib::SingletonFactory<sw::redis::Redis>::Instance();
    instance->hset(SESSION_KEY_TABLE, user_id, session_key);
    return RetCode::SUCCESS;
}
