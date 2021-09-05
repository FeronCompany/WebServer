

#pragma once

#include "user.pb.h"
#include "ProError.h"

class UserInfoDBUtil
{
public:
    static RetCode UserInfoRegist(const cws::user::UserInfo& userInfo);

    static RetCode UserInfoGet(const std::string& uid, cws::user::UserInfo& userInfo);
};
