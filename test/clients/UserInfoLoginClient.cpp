

#include "CommonLib.h"
#include "CwsChannel.h"
#include "CwsController.h"

#include "user_service.pb.h"
#include "UserInfoLoginClient.h"

namespace CWSTest
{
    int32_t UserInfoLoginClient::Execute()
    {
        CwsFrame::Channel chann;
        chann.Init("127.0.0.1", 9091);
        CwsFrame::Controller controller;
        cws::user::user_info_regist_rqst rqst;
        rqst.set_user_name("wen");
        rqst.set_phone_number("18344447777");
        rqst.set_password("999666");
        rqst.set_gender(cws::common::Gender::MALE);
        cws::user::user_info_regist_resp resp;
        cws::user::UserService_Stub stub(&chann);
        stub.user_info_regist(&controller, &rqst, &resp, nullptr);
        NORMAL_LOG("user_info_regist retcode[%d], ret_msg[%s], id[%s]",
            resp.ret_info().err_code(),
            resp.ret_info().err_msg().c_str(),
            resp.user_id().c_str());
        if (resp.ret_info().err_code() != 0)
        {
            return -1;
        }

        std::string user_id = resp.user_id();
        cws::user::user_info_login_rqst rqst_login;
        rqst_login.set_id(user_id);
        rqst_login.set_password("999666");
        cws::user::user_info_login_resp resp_login;
        stub.user_info_login(&controller, &rqst_login, &resp_login, nullptr);
        NORMAL_LOG("user_info_login retcode[%d], ret_msg[%s], session_key[%s]",
            resp_login.ret_info().err_code(),
            resp_login.ret_info().err_msg().c_str(),
            resp_login.session_key().c_str());
        if (resp_login.ret_info().err_code() != 0)
        {
            return -1;
        }
        return 0;
    }
    REG_CLIENT_TASK(UserInfoLoginClient);
}
