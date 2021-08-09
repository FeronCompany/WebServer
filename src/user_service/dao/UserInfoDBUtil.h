

#pragma once

#include "build/stubs/user.pb.h"
#include "src/proj_comm/ProError.h"

class UserInfoDBUtil
{
public:
    static RetCode UserInfoRegist(const cws::user::UserInfo& userInfo);
};
