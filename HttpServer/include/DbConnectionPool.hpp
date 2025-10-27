/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-17 13:14:08
 * @LastEditors: cyclone cyclone@gmail.com
 * @LastEditTime: 2025-10-07 15:30:23
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/DbConnectionPool.hpp
 * @Description:
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */

#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include "DbConnection.hpp"

namespace http::db 
{

class DbConnectionPool 
{
public:

    static auto getInstance() -> DbConnectionPool& 
    {
        static DbConnectionPool instance;
        return instance;
    }


    void init(const std::string& host,
             const std::string& user,
             const std::string& password,
             const std::string& database,
             size_t poolSize = 10);

    /**
     * @brief 从连接池获取数据库连接
     * @return 包装了自动归还机制的数据库连接智能指针
     * @throws DbException 当连接池未初始化或连接失败时
     */
    auto getConnection() -> std::shared_ptr<DbConnection>;
    
    // 禁止拷贝构造和赋值操作，维护单例模式的完整性
    DbConnectionPool(const DbConnectionPool&) = delete;
    auto operator=(const DbConnectionPool&) -> DbConnectionPool& = delete;

private:
    /**
     * @brief 私有构造函数，实现单例模式
     * 
     * 初始化后台连接检查线程，确保连接池的高可用性
     */
    DbConnectionPool();
    
    /**
     * @brief 析构函数，清理所有连接资源
     * 
     * 自动释放所有连接，智能指针保证资源正确清理
     */
    ~DbConnectionPool();

    auto createConnection() -> std::shared_ptr<DbConnection>;
    
    /**
     * @brief 后台连接健康监控线程
     * 检查策略：
     * - 检查频率：60秒/次，平衡性能和实时性
     * - 检查方式：复制队列后在锁外执行，降低锁争用
     */
    void checkConnections();

    // 数据库连接参数，用于创建新连接和重连
    std::string                               host_;      ///< 数据库主机地址
    std::string                               user_;      ///< 数据库用户名
    std::string                               password_;  ///< 数据库密码  
    std::string                               database_;  ///< 数据库名称
    
    // 连接池核心数据结构
    std::queue<std::shared_ptr<DbConnection>> connections_; ///< 连接队列，存储可用连接
    
    // 线程同步
    std::mutex                                mutex_;     ///< 保护连接池的互斥锁
    std::condition_variable                   cv_;        ///< 协调连接分配与归还的条件变量
    
    // 状态管理
    bool                                      initialized_ = false; ///< 初始化标志，防止重复初始化
    std::thread                               checkThread_;         ///< 后台连接检查线程
};

} // namespace http
