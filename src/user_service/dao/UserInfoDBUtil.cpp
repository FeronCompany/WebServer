

#include "CommonLib.h"

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

RetCode UserInfoDBUtil::UserInfoGet(const std::string& uid, cws::user::UserInfo& userInfo)
{
    if (uid.empty())
    {
        ERROR_LOG("uid is empty.");
        return RetCode::PARAM_ERROR;
    }
    std::string tableName = "cws_user.user_info";
    std::string rows = "user_uid, oid, user_name, phone_number, gender, birthday, password, create_time";
    std::string queryString = CWSLib::String::format(
        "SELECT %s FROM %s WHERE oid='%s'",
        rows.c_str(),
        tableName.c_str(),
        uid.c_str());
    auto instance = CWSLib::CommSingleton<CWSLib::MySQLConnectionPool>::instance()->getConnection();
    NORMAL_LOG("mysql command: %s.", queryString.c_str());
    int res = mysql_real_query(instance, queryString.c_str(), queryString.length());
    if (res)
    {
        ERROR_LOG("Query user info fail %s! ", mysql_error(instance));
        return RetCode::QUERY_USER_FAILED;
    }
    auto mysql_res = mysql_store_result(instance);
    MYSQL_ROW mysql_row;
    mysql_row = mysql_fetch_row(mysql_res);
    if (!mysql_row)
    {
        ERROR_LOG("User %s not exist.", uid.c_str());
        mysql_free_result(mysql_res);
        return RetCode::NOT_EXIST;
    }
    while (mysql_row)
    {
        userInfo.set_user_uid(mysql_row[0]);
        userInfo.set_oid(mysql_row[1]);
        userInfo.set_user_name(mysql_row[2]);
        userInfo.set_phone_number(mysql_row[3]);
        userInfo.set_gender((cws::common::Gender)atoi(mysql_row[4]));
        userInfo.set_birthday(mysql_row[5]);
        userInfo.set_password(mysql_row[6]);
        userInfo.set_create_time(mysql_row[7]);
        mysql_row = mysql_fetch_row(mysql_res);
    }
    mysql_free_result(mysql_res);
    return RetCode::SUCCESS;
}
