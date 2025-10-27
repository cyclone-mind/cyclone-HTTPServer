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
    /*
    
    C语言中的getopt函数来解析命令行参数。
    代码的主要目的是从命令行参数中获取指定的参数值，并将其存储在变量port中。
    */
    
int opt; // 声明一个整型变量opt，用于存储getopt函数返回的命令行选项。
    const char* str = "p:"; // 定义一个字符串str，它指定了要解析的命令行选项。这里"p:"表示要查找的命令行选项是-p，后面的冒号":"表示该选项后面跟有一个参数。
    while ((opt = getopt(argc, argv, str)) != -1)
    {
        // 循环条件，它会持续调用getopt函数来解析命令行参数，直到没有更多的参数需要解析为止。getopt函数会返回下一个命令行选项的字符代码，如果没有更多的选项，它会返回-1。
        switch (opt)
        {
        case 'p':
        {
            port = atoi(optarg); // 将-p选项后面的参数转换为整数，并存储在变量port中。optarg是getopt函数提供的指针，它指向当前选项的参数。
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
    // 如果需要保证每个线程处理相同的业务逻辑，也可以再加一个队列，线程从队列中获取任务并执行
    RabbitMQThreadPool pool(RABBITMQ_HOST, QUEUE_NAME, THREAD_NUM, executeMysql);
    pool.start();

    server.start();
}
