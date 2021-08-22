

#include "MySQLPool.h"
#include "CwsServer.h"

#include "ProJobImpl.h"
#include "UserInfoRegist.h"
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
    server.AddService("UserService", userService);

    // init config
    UserServiceConfig userConfig("../config/user_service_config.json");
    auto raw_config = userConfig.Config();

    // init mysql connection
    CWSLib::CommSingleton<CWSLib::MySQLConnectionPool>::instance()->init(
        raw_config->db_config().ip(),
        raw_config->db_config().port(),
        raw_config->db_config().user_name(),
        raw_config->db_config().password(),
        "",
        raw_config->db_config().pool_size());
    return 0;
}
