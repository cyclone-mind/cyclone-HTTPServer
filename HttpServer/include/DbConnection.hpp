/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-17 12:16:30
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 15:13:20
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/DbConnection.hpp
 * @Description:
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#pragma once
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <muduo/base/Logging.h>
#include <mysql_connection.h>
#include <mysql_driver.h>

#include <memory>
#include <mutex>
#include <string>

#include "DbException.hpp"

namespace http::db {
class DbConnection {
private:
    std::shared_ptr<sql::Connection> conn_;
    std::string host_;
    std::string user_;
    std::string password_;
    std::string database_;
    std::mutex mutex_;

public:
    DbConnection(std::string host, std::string user, std::string password, std::string database);
    ~DbConnection() = default;

    // 禁止拷贝
    DbConnection(const DbConnection&) = delete;
    auto operator=(const DbConnection&) -> DbConnection& = delete;

    // 模板方法必须放在头文件中 - 返回类型为智能指针
    template <typename... Args>
    auto executeQuery(const std::string& sql, Args&&... args) -> std::unique_ptr<sql::ResultSet> {
        // 线程安全保护
        std::lock_guard<std::mutex> lock(mutex_);
        try {
            // 检查连接有效性，如果无效则重连，但不调用cleanup避免状态冲突
            if (!conn_ || !conn_->isValid()) {
                reconnect();
            }
            
            // 直接创建新的预处理语句，不使用缓存
            std::unique_ptr<sql::PreparedStatement> stmt(conn_->prepareStatement(sql));
            // 绑定参数
            bindParams(stmt.get(), 1, std::forward<Args>(args)...);
            
            // 使用智能指针管理结果集
            std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
            return result;
        } catch (const sql::SQLException& e) {
            LOG_ERROR << "Query failed: " << e.what() << ", SQL: " << sql;
            throw DbException(e.what());
        }
    }

    template <typename... Args>
    auto executeUpdate(const std::string& sql, Args&&... args) -> int {
        // 线程安全保护
        std::lock_guard<std::mutex> lock(mutex_);
        try {
            // 检查连接有效性
            if (!conn_ || !conn_->isValid()) {
                reconnect();
            }
            
            // 直接创建新的预处理语句，不使用缓存
            std::unique_ptr<sql::PreparedStatement> stmt(conn_->prepareStatement(sql));
            // 绑定参数
            bindParams(stmt.get(), 1, std::forward<Args>(args)...);
            return stmt->executeUpdate();
        } catch (const sql::SQLException& e) {
            LOG_ERROR << "Update failed: " << e.what() << ", SQL: " << sql;
            throw DbException(e.what());
        }
    }

    auto isValid() -> bool;
    void reconnect();
    void cleanup();
    auto ping() -> bool;

private:
    // 辅助函数：递归终止条件
    void bindParams(sql::PreparedStatement*, int) {}

    // 辅助函数：绑定参数 - 通用类型
    template <typename T, typename... Args>
    void bindParams(sql::PreparedStatement* stmt, int index, T&& value, Args&&... args) {
        setParam(stmt, index, std::forward<T>(value));
        bindParams(stmt, index + 1, std::forward<Args>(args)...);
    }

    // 参数设置的具体实现
    void setParam(sql::PreparedStatement* stmt, int index, const std::string& value) {
        stmt->setString(index, value);
    }
    
    void setParam(sql::PreparedStatement* stmt, int index, int value) {
        stmt->setInt(index, value);
    }
    
    void setParam(sql::PreparedStatement* stmt, int index, bool value) {
        stmt->setBoolean(index, value);
    }
    
    void setParam(sql::PreparedStatement* stmt, int index, double value) {
        stmt->setDouble(index, value);
    }
    
    // 其他类型转换为字符串
    template<typename T>
    void setParam(sql::PreparedStatement* stmt, int index, T&& value) {
        stmt->setString(index, std::to_string(std::forward<T>(value)));
    }
};
}  // namespace http::db