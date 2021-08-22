

#ifndef __SERVICE_UTILS_H__
#define __SERVICE_UTILS_H__

#include <string>
#include <memory>

#include "user.pb.h"

class UserServiceConfig
{
public:
    UserServiceConfig(const std::string& configPath);
    std::shared_ptr<cws::user::UserServiceConfig> Config();

private:
    std::shared_ptr<cws::user::UserServiceConfig> mUserConfig;
};

#endif // __SERVICE_UTILS_H__
