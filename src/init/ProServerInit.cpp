

#include <sw/redis++/redis++.h>

#include "MySQLPool.h"
#include "CwsServer.h"

#include "ProJobImpl.h"
#include "UserInfoRegist.h"
#include "UserInfoLogin.h"
#include "ServiceUtils.h"

#include "ProServerInit.h"

int ServerInit()
{
    // init jobs
    auto& jobsFactory = *CJobFactory::instance();
    jobsFactory.join("pro_job_impl", []() { return new ProJobImpl; }, true);

    // init services and methods
    auto& server = *CServerSingleton::instance();
    CwsFrame::Service* userService = new CwsFrame::Service("UserService");
    userService->AddMethod("user_info_regist", []() { return std::shared_ptr<UserInfoRegist>(new UserInfoRegist); });
    userService->AddMethod("user_info_login", []() { return std::shared_ptr<UserInfoLogin>(new UserInfoLogin); });
    server.AddService("UserService", userService);

    // init config
    UserServiceConfig userConfig("../config/user_service_config.json");
    auto rawConfig = userConfig.Config();

    // init mysql connection
    CWSLib::CommSingleton<CWSLib::MySQLConnectionPool>::instance()->init(
        rawConfig->db_config().ip(),
        rawConfig->db_config().port(),
        rawConfig->db_config().user_name(),
        rawConfig->db_config().password(),
        "",
        rawConfig->db_config().pool_size());

    sw::redis::ConnectionOptions connectionOptions;
    connectionOptions.host = rawConfig->redis_config().ip();
    connectionOptions.port = rawConfig->redis_config().port();
    connectionOptions.connect_timeout = std::chrono::milliseconds(200);
    connectionOptions.socket_timeout = std::chrono::milliseconds(200);
    sw::redis::ConnectionPoolOptions poolOptions;
    poolOptions.size = rawConfig->redis_config().pool_size();
    CWSLib::SingletonFactory<sw::redis::Redis>::CreateUnique(connectionOptions, poolOptions);
    return 0;
}
