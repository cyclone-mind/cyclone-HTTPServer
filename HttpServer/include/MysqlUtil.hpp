/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-17 13:52:28
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 14:02:44
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/MysqlUtil.hpp
 * @Description:
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */

#pragma once
#include "DbConnectionPool.hpp"
namespace http {
class MysqlUtil {
public:
    static void init(const std::string& host, const std::string& user, const std::string& password,
                     const std::string& dbname, size_t poolSize) {
        db::DbConnectionPool::getInstance().init(host, user, password, dbname, poolSize);
    }

    template <typename... Args>
    static auto executeQuery(const std::string& sql, Args&&... args) -> std::unique_ptr<sql::ResultSet> {
        auto conn = http::db::DbConnectionPool::getInstance().getConnection();
        return conn->executeQuery(sql, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static auto executeUpdate(const std::string& sql, Args&&... args) -> int {
        auto conn = http::db::DbConnectionPool::getInstance().getConnection();
        return conn->executeUpdate(sql, std::forward<Args>(args)...);
    }
};
}  // namespace http