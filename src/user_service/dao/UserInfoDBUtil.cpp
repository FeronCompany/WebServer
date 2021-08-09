

#include "commlib/db/MySQLPool.h"
#include "commlib/basic/StringUtils.h"
#include "commlib/basic/CommonSingleton.h"
#include "commlib/app/MacroAssemble.h"

#include "UserInfoDBUtil.h"

RetCode UserInfoDBUtil::UserInfoRegist(const cws::user::UserInfo& userInfo)
{
    std::string birthday = userInfo.birthday().empty() ? "1970-01-01 00:00:00" : userInfo.birthday();
    std::string tableName = "cws_user.user_info";
    std::string rows = "(user_uid, oid, user_name, phone_number, gender, birthday, password, create_time)";
    std::string value = CWSLib::String::format(
        "('%s','%s','%s','%s',%d,'%s','%s',now())",
        userInfo.user_uid().c_str(),
        userInfo.oid().c_str(),
        userInfo.user_name().c_str(),
        userInfo.phone_number().c_str(),
        (int)userInfo.gender(),
        birthday.c_str(),
        userInfo.password().c_str());
    std::string queryString = CWSLib::String::format(
        "INSERT INTO %s %s VALUES %s",
        tableName.c_str(),
        rows.c_str(),
        value.c_str());
#if 0
    std::string queryString = CWSLib::String::append(
        "INSERT INTO %s %s VALUES %s",
        tableName,
        rows,
        value);
#endif
    auto instance = CWSLib::CommSingleton<CWSLib::MySQLConnectionPool>::instance()->getConnection();
    NORMAL_LOG("mysql command: %s.", queryString.c_str());
    int res = mysql_real_query(instance, queryString.c_str(), queryString.length());
    if (res)
    {
        ERROR_LOG("Regist user info fail %s! ", mysql_error(instance));
        return RetCode::REGIST_USER_FAILED;
    }
    return RetCode::SUCCESS;
}
