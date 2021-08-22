

#pragma once

#include "user.pb.h"
#include "ProError.h"

class UserInfoDBUtil
{
public:
    static RetCode UserInfoRegist(const cws::user::UserInfo& userInfo);
};
