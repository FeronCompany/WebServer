

#pragma once

#include <mysql/mysql.h>
#include <mutex>
#include <memory>
#include <vector>
#include <string>

namespace CWSLib
{
    class MySQLConnectionPool
    {
    public:
        MySQLConnectionPool();
        ~MySQLConnectionPool();
        void init(
            const std::string& ip,
            const uint32_t port,
            const std::string& user,
            const std::string& passwd,
            const std::string& db,
            const uint32_t min_size);
        MYSQL* getConnection();
        int getSize();
    private:
        std::mutex mConnLock;
        std::vector<std::shared_ptr<MYSQL>> mConnList;
        int mConnIndex;
    };
}
