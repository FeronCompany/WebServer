

#ifndef __USER_INFO_LOGIN_CLIENT_H__
#define __USER_INFO_LOGIN_CLIENT_H__

#include "ClientsJobQueue.h"

namespace CWSTest
{
    class UserInfoLoginClient : public ClientJobBase
    {
    public:
        int32_t Execute() override;
    };
}

#endif // !__USER_INFO_LOGIN_CLIENT_H__

