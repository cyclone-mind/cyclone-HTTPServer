/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-17 13:19:23
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 15:28:36
 * @FilePath: /cyclone-HTTPServer/HttpServer/src/DbConnectionPool.cpp
 * @Description:
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */

#include "../include/DbConnectionPool.hpp"
#include "../include/DbException.hpp"
#include <muduo/base/Logging.h>

namespace http::db {

void DbConnectionPool::init(const std::string& host, const std::string& user,
                            const std::string& password, const std::string& database,
                            size_t poolSize) {
    // 连接池会被多个线程访问，使用RAII锁管理确保线程安全
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 确保只初始化一次，防止重复初始化导致的资源浪费
    if (initialized_) {
        return;
    }

    // 保存数据库连接参数，用于后续创建新连接和重连
    host_ = host;
    user_ = user;
    password_ = password;
    database_ = database;

    // 预先创建指定数量的连接放入队列中
    // 这是对象池模式的核心：预分配资源以提高性能
    for (size_t i = 0; i < poolSize; ++i) {
        connections_.push(createConnection());
    }

    initialized_ = true;
    LOG_INFO << "Database connection pool initialized with " << poolSize << " connections";
}


/**
 * @brief 连接池构造函数
 * 
 * 设计原理：
 * 1. 私有构造函数配合单例模式，确保全局唯一实例
 * 2. 启动后台健康检查线程，定期检查连接池中的连接状态
 * 3. 使用分离线程避免阻塞主线程
 */
DbConnectionPool::DbConnectionPool() {
    // 启动后台健康检查线程
    // 使用改进的检查策略：在锁外检查连接，避免与正在使用的连接冲突
    checkThread_ = std::thread(&DbConnectionPool::checkConnections, this);
    checkThread_.detach();
}


DbConnectionPool::~DbConnectionPool() {
    // 获取锁确保线程安全地清理资源
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 清空连接队列，智能指针会自动释放连接资源
    while (!connections_.empty()) {
        connections_.pop();
    }
    
    LOG_INFO << "Database connection pool destroyed";
}

/**
 * @brief 从连接池获取数据库连接
 * @return 包装了自动归还功能的智能指针
 */
auto DbConnectionPool::getConnection() -> std::shared_ptr<DbConnection> {
    std::shared_ptr<DbConnection> conn;
    {
        std::unique_lock<std::mutex> lock(mutex_);

        // 等待可用连接，实现生产者-消费者模式的消费者逻辑
        while (connections_.empty()) {
            // 防御性检查：确保连接池已初始化
            if (!initialized_) {
                throw DbException("Connection pool not initialized");
            }
            
            LOG_INFO << "Waiting for available connection...";
            // 阻塞等待，直到有连接可用或被唤醒
            cv_.wait(lock);
        }

        // 从队列头部获取连接
        conn = connections_.front();
        connections_.pop();
    } 

    try {
        // 在锁外检查连接有效性，避免长时间持锁
        // 这是一种主动式的连接管理策略
        if (!conn->ping()) {
            LOG_WARN << "Connection lost, attempting to reconnect...";
            conn->reconnect();
        }
        
        // 返回带有自定义删除器的智能指针
        // 删除器负责将连接归还到池中，实现RAII模式
        // 这确保了无论在什么情况下，连接都会被正确归还
        return std::shared_ptr<DbConnection>(conn.get(), [this, conn](DbConnection*) {
            // 归还连接到池中
            std::lock_guard<std::mutex> lock(mutex_);
            connections_.push(conn);
            // 通知等待连接的线程
            cv_.notify_one();
        });
    } catch (const std::exception& e) {
        LOG_ERROR << "Failed to get connection: " << e.what();
        // 异常情况下也要归还连接，确保异常安全
        {
            std::lock_guard<std::mutex> lock(mutex_);
            connections_.push(conn);
            cv_.notify_one();
        }
        throw;
    }
}


auto DbConnectionPool::createConnection() -> std::shared_ptr<DbConnection> {
    return std::make_shared<DbConnection>(host_, user_, password_, database_);
}

/**
 * @brief 后台连接健康检查线程
 * 
 * 设计原理（改进后）：
 * 1. 主动式连接管理：定期检查连接健康状态，预防连接失效
 * 2. 非阻塞设计：在锁外执行耗时的ping和重连操作
 * 3. 分离检查：只检查池中闲置的连接，避免与正在使用的连接冲突
 * 4. 异常恢复：捕获异常并继续运行，确保监控线程的稳定性
 * 5. 合理的检查频率：平衡系统开销和连接可用性
 */
void DbConnectionPool::checkConnections() {
    // 无限循环，持续监控连接状态
    while (true) {
        try {
            std::vector<std::shared_ptr<DbConnection>> connsToCheck;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                
                // 如果没有连接可检查，短暂休眠后继续
                if (connections_.empty()) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    continue;
                }

                // 复制连接队列，避免在检查时长时间持锁
                // 这是一种读取时复制的策略，提高并发性能
                auto temp = connections_;
                while (!temp.empty()) {
                    connsToCheck.push_back(temp.front());
                    temp.pop();
                }
            } // 释放锁，避免在检查连接时阻塞其他操作

            // 在锁外执行耗时的连接检查操作
            // 这样不会阻塞其他线程获取连接
            for (auto& conn : connsToCheck) {
                // 检查连接是否仍然有效
                if (!conn->ping()) {
                    try {
                        // 尝试重新建立连接
                        LOG_WARN << "Connection health check failed, attempting to reconnect...";
                        conn->reconnect();
                        LOG_INFO << "Connection successfully reconnected";
                    } catch (const std::exception& e) {
                        LOG_ERROR << "Failed to reconnect: " << e.what();
                        // 重连失败，连接可能需要被替换
                        // 但为了简化设计，这里只记录错误
                    }
                }
            }

            // 每60秒检查一次，平衡检查频率和系统开销
            std::this_thread::sleep_for(std::chrono::seconds(60));
            
        } catch (const std::exception& e) {
            // 捕获所有异常，确保监控线程不会因异常而终止
            LOG_ERROR << "Error in connection check thread: " << e.what();
            // 异常后短暂休眠，避免异常循环
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
}
}  // namespace http::db