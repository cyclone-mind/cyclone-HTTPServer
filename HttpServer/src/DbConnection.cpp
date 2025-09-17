/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-17 12:20:28
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 14:52:51
 * @FilePath: /cyclone-HTTPServer/HttpServer/src/DbConnection.cpp
 * @Description:
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */

#include "../include/DbConnection.hpp"

namespace http::db {
DbConnection::DbConnection(std::string host, std::string user, std::string password,
                           std::string database)
    : host_(std::move(host)),
      user_(std::move(user)),
      password_(std::move(password)),
      database_(std::move(database)) {
    try {
        // 获取MySQL驱动实例
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        // 建立数据库连接
        conn_.reset(driver->connect(host_, user_, password_));
        if (conn_) {
            // 设置数据库schema
            conn_->setSchema(database_);

            // 设置连接属性
            conn_->setClientOption("OPT_RECONNECT", "true");
            conn_->setClientOption("OPT_CONNECT_TIMEOUT", "10");
            conn_->setClientOption("multi_statements", "false");

            // 设置字符集
            std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
            stmt->execute("SET NAMES utf8mb4");

            LOG_INFO << "Database connection established";
        }
    } catch (const sql::SQLException& e) {
        LOG_ERROR << "Failed to create database connection: " << e.what();
        throw DbException(e.what());
    }
}


auto DbConnection::isValid() -> bool {
    try {
        // 检查连接对象是否存在
        if (!conn_) {
            return false;
        }
        // 执行简单查询测试连接有效性
        //普通语句 (Statement)：直接执行SQL，如果有用户输入需要手动处理安全问题
        std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
        
        // execute方法--用途：执行 任何类型 的SQL语句
        // 返回值：bool - 表示是否有结果集返回，只关心执行成功与否
        stmt->execute("SELECT 1");
        return true;
    } catch (const sql::SQLException&) {
        return false;
    }
}

void DbConnection::reconnect() {
    try {
        LOG_INFO << "Attempting to reconnect to database...";
        
        // 重新获取驱动实例
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        
        // 关闭旧连接
        if (conn_) {
            try {
                conn_->close();
            } catch (...) {
                // 忽略关闭时的错误
            }
        }
        
        // 创建新连接
        conn_.reset(driver->connect(host_, user_, password_));
        if (conn_) {
            // 设置数据库schema
            conn_->setSchema(database_);
            
            // 设置连接属性
            conn_->setClientOption("OPT_CONNECT_TIMEOUT", "10");
            conn_->setClientOption("multi_statements", "false");
            
            // 设置字符集
            std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
            stmt->execute("SET NAMES utf8mb4");
            
            LOG_INFO << "Database reconnection successful";
        }
    } catch (const sql::SQLException& e) {
        LOG_ERROR << "Reconnect failed: " << e.what();
        throw DbException(e.what());
    }
}
auto DbConnection::ping() -> bool {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        // 简单检查连接对象是否存在且有效
        return conn_ && conn_->isValid();
    } catch (const sql::SQLException& e) {
        LOG_DEBUG << "Ping failed: " << e.what();
        return false;
    }
}
void DbConnection::cleanup() {
    // 注意：这个方法可能在锁内被调用，所以不要再次获取锁
    try {
        if (conn_) {
            // 确保所有事务都已完成
            if (!conn_->getAutoCommit()) {
                conn_->rollback();
                conn_->setAutoCommit(true);
            }
        }
    } catch (const std::exception& e) {
        LOG_WARN << "Error cleaning up connection: " << e.what();
        // 清理失败时不要抛异常，避免连锁反应
    }
}

}  // namespace http::db