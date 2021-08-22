

#include "MacroAssemble.h"
#include "Exception.h"
#include "MySQLPool.h"

namespace CWSLib
{
    MySQLConnectionPool::MySQLConnectionPool()
    {
        mConnIndex = 0;
    }

    MySQLConnectionPool::~MySQLConnectionPool()
    {
        std::lock_guard<std::mutex> guard(mConnLock);
        for (auto conn : mConnList)
        {
            mysql_close(conn.get());
        }
    }

    void MySQLConnectionPool::init(
        const std::string& ip,
        const uint32_t port,
        const std::string& user,
        const std::string& passwd,
        const std::string& db,
        const uint32_t min_size)
    {
        for (uint32_t i = 0; i < min_size; i++)
        {
            MYSQL* mysqlPtr = new MYSQL();
            mysql_init(mysqlPtr);
            bool reconnect = true;
            mysql_options(mysqlPtr, MYSQL_OPT_RECONNECT, &reconnect);
            if (!mysql_real_connect(mysqlPtr, ip.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, NULL, CLIENT_FOUND_ROWS))
            {
                ERROR_LOG("Failed to connect to database, %s", mysql_error(mysqlPtr));
                throw Exception(-1, mysql_error(mysqlPtr));
            }
            else
            {
                mConnList.push_back(std::shared_ptr<MYSQL>(mysqlPtr));
            }
        }
    }

    MYSQL* MySQLConnectionPool::getConnection()
    {
        std::lock_guard<std::mutex> guard(mConnLock);
        if (mConnList.size() == 0)
        {
            ERROR_LOG("Connection pool is empty!");
            throw Exception(-2, "Connection pool is empty");
        }
        if (mConnIndex >= mConnList.size())
        {
            mConnIndex = 0;
        }
        MYSQL* conn = mConnList[mConnIndex].get();
        mConnIndex++;
        return conn;
    }

    int MySQLConnectionPool::getSize()
    {
        return mConnList.size();
    }

} // namespace CWSLib

