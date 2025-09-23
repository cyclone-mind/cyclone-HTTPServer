#pragma once

#include <SimpleAmqpClient/SimpleAmqpClient.h>  // RabbitMQ客户端库
#include <vector>                                // 动态数组容器
#include <mutex>                                 // 互斥锁
#include <memory>                                // 智能指针
#include <atomic>                                // 原子操作
#include <thread>                                // 线程支持
#include <iostream>                              // 输入输出流
#include <chrono>                                // 时间处理
#include <functional>                            // 函数对象

// 消息队列管理器类，使用单例模式实现连接池管理
class MQManager {
public:
    // 获取单例实例
    static MQManager& instance() {
        static MQManager mgr;
        return mgr;
    }

    // 发布消息到指定队列
    void publish(const std::string& queue, const std::string& msg);

private:
    // 消息队列连接结构体，封装通道和互斥锁
    struct MQConn {
        AmqpClient::Channel::ptr_t channel;  // RabbitMQ通道指针
        std::mutex mtx;                      // 通道操作互斥锁
    };

    MQManager(size_t poolSize = 5);          // 构造函数，默认连接池大小为5

    // 禁止拷贝构造和赋值操作
    MQManager(const MQManager&) = delete;
    MQManager& operator=(const MQManager&) = delete;

    std::vector<std::shared_ptr<MQConn>> pool_;  // 连接池
    size_t poolSize_;                            // 连接池大小
    std::atomic<size_t> counter_;                // 原子计数器，用于轮询选择连接
};

// RabbitMQ 消息消费线程池类，用于多线程消费消息
class RabbitMQThreadPool {
public:
    using HandlerFunc = std::function<void(const std::string&)>;  // 消息处理函数类型

    RabbitMQThreadPool(const std::string& host,
        const std::string& queue,
        int thread_num,
        HandlerFunc handler)
        : stop_(false),
        rabbitmq_host_(host),
        queue_name_(queue),
        thread_num_(thread_num),
        handler_(handler) {}

    void start();     // 启动线程池
    void shutdown();  // 关闭线程池

    // 析构函数自动关闭线程池
    ~RabbitMQThreadPool() {
        shutdown();
    }

private:
    void worker(int id);  // 工作线程函数

private:
    std::vector<std::thread> workers_;    // 工作线程容器
    std::atomic<bool> stop_;              // 停止标志，原子操作保证线程安全
    std::string queue_name_;              // 队列名称
    int thread_num_;                      // 线程数量
    std::string rabbitmq_host_;           // RabbitMQ服务器地址
    HandlerFunc handler_;                 // 消息处理回调函数
};
