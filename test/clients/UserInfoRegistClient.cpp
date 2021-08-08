

#include <unistd.h>
#include <cstdlib>

#include "commlib/app/MacroAssemble.h"
#include "frame/CwsChannel.h"
#include "frame/CwsController.h"

#include "build/stubs/user_service.pb.h"
#include "UserInfoRegistClient.h"

namespace CWSTest
{
    int32_t UserInfoRegistClient::Execute()
    {
        for (int i = 0; i < 10; ++i)
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
            NORMAL_LOG("retcode[%d], ret_msg[%s], id[%s]",
                resp.ret_info().err_code(),
                resp.ret_info().err_msg().c_str(),
                resp.user_id().c_str());
            if (resp.ret_info().err_code() != 0)
            {
                return -1;
            }
            sleep(1);
        }

        return 0;
    }

    REG_CLIENT_TASK(UserInfoRegistClient);
}

