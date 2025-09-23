#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>

#include"../include/ChatServer.h"

const std::string RABBITMQ_HOST = "localhost";
const std::string QUEUE_NAME = "sql_queue";
const int THREAD_NUM = 2;

void executeMysql(const std::string sql) {
    http::MysqlUtil mysqlUtil_;
    mysqlUtil_.executeUpdate(sql);
}


int main(int argc, char* argv[]) {
	LOG_INFO << "pid = " << getpid();
	std::string serverName = "ChatServer";
	int port = 80;
    // 解析命令行参数
    int opt;
    const char* str = "p:";
    while ((opt = getopt(argc, argv, str)) != -1)
    {
        switch (opt)
        {
        case 'p':
        {
            port = atoi(optarg);
            break;
        }
        default:
            break;
        }
    }
    muduo::Logger::setLogLevel(muduo::Logger::WARN);
    ChatServer server(port, serverName);
    server.setThreadNum(4);
    //这里需要加个睡眠等待，因为 ChatServer 构造函数中初始化数据库连接
    std::this_thread::sleep_for(std::chrono::seconds(2));
    //初始化chat_message表和 chatInformation 表
    server.initChatMessage();    

    // 启动RabbitMQ线程池，负责处理SQL队列中的任务，线程池统一处理业务逻辑
    //如果需要保证每个线程处理相同的业务逻辑，也可以再加一个队列，线程从队列中获取任务并执行
    RabbitMQThreadPool pool(RABBITMQ_HOST, QUEUE_NAME, THREAD_NUM, executeMysql);
    pool.start();

    server.start();
}
