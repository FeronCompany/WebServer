

#include <fstream>
#include <sstream>

#include "CommonLib.h"
#include "ServiceUtils.h"

UserServiceConfig::UserServiceConfig(const std::string& configPath)
{
    mUserConfig.reset(new cws::user::UserServiceConfig);
    std::ifstream ifs(configPath);
    if (!ifs)
    {
        ERROR_LOG("Open file %s failed.", configPath.c_str());
        return;
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    CWSLib::JsonStringToProto(ss.str(), *mUserConfig);
}

std::shared_ptr<cws::user::UserServiceConfig> UserServiceConfig::Config()
{
    return mUserConfig;
}
