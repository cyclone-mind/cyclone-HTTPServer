/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-18 10:20:23
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-18 16:29:10
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/SslConnection.hpp
 * @Description:
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#pragma once
#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>

#include <functional>

#include "SslContext.hpp"
#include "muduo/net/TcpConnection.h"

namespace ssl {
using MessageCallback =
    std::function<void(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp)>;
class SslConnection {
public:
    using TcpConnectionPtr = std::shared_ptr<muduo::net::TcpConnection>;
    using BufferPtr = muduo::net::Buffer*;

    // SslConnection 构造的时候把tcp的消息回调换成本层 ssl的函数 SslConnection::onRead，
    // 也就是说，tcp消息到来时会先调用ssl的回调 SslConnection::onRead，
    // 然后在 SslConnection::onRead 中调用 ssl::MessageCallback
    SslConnection(const TcpConnectionPtr& conn, SslContext* ctx);
    ~SslConnection();

    // 因为 ssl层处理完后要交给http层，因此通过此函数将http层的消息回调 HttpServer::onMessage
    // 保存下来， 然后在 SslConnection::onRead 中调用。
    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb;}
    // ssl 的回调
    void onRead(const TcpConnectionPtr& conn, BufferPtr buf, muduo::Timestamp time);
    // 启动握手过程 只在连接建立时调用一次
    // 设置SSL为服务器接受模式，
    // 然后调用 handleHandshake() 处理单次握手
    void startHandshake();

    // 在 HttpServer::onRequest 中处理完业务，响应封装完成后 http 层根据 tcp 链接找到 ssl
    // 链接，调用该方法。 该方法会加密后发送给 tcp 层
    void send(const void* data, size_t len);

private:
    // 处理单次握手
    void handleHandshake();
    
    // 发送待发送的SSL数据
    void sendPendingData();
    
    auto getLastError(int ret) -> SSLError;
    void handleError(SSLError error);

    SSL* ssl_;                            // SSL 连接
    SslContext* ctx_;                     // SSL 上下文
    TcpConnectionPtr conn_;               // TCP 连接
    SSLState state_;                      // SSL 状态，独立于TCP连接状态
    BIO* readBio_;                        // 接收从 TCP 层来的数据，供 SSL读取
    BIO* writeBio_;                       // 存储 SSL加密后的数据，写入 TCP 层发送
    muduo::net::Buffer readBuffer_;       // 读缓冲区
    muduo::net::Buffer writeBuffer_;      // 写缓冲区
    muduo::net::Buffer decryptedBuffer_;  // 解密后的数据
    MessageCallback messageCallback_;     // 消息回调
};
}  // namespace ssl

/*
理解 BIO 在 TCP 层和 SSL 层之间的数据传递

readBio_的作用：
    BIO_write(readBio_, 网络接收的加密数据);  // 输入给SSL
    SSL_read(ssl_, 明文缓冲区);               // SSL从readBio_读取并解密

writeBio_的作用：
    SSL_write(ssl_, 明文数据);               // SSL加密并写入writeBio_
    BIO_read(writeBio_, 加密数据缓冲区);      // 从writeBio_取出加密数据
    conn_->send(加密数据);                   // 发送到网络


完整的数据流示例

## 客户端发送 "GET / HTTP/1.1"

【客户端】                     【服务器】
应用: "GET / HTTP/1.1"          
   ↓ SSL_write()
SSL: [加密后的字节]             
   ↓ 网络传输                   
                               TCP: 接收[加密字节] → buf
                                ↓ BIO_write(readBio_, buf)
                               SSL: readBio_存储[加密字节]
                                ↓ SSL_read()  
                               SSL: 解密 → "GET / HTTP/1.1"
                                ↓ messageCallback_()
                               HTTP: 处理"GET / HTTP/1.1"

## 服务器回复 "HTTP/1.1 200 OK"

【服务器】                     【客户端】
HTTP: "HTTP/1.1 200 OK"        
   ↓ SslConnection::send()
SSL: SSL_write() → writeBio_[加密字节]
   ↓ BIO_read(writeBio_)
TCP: conn_->send([加密字节])
   ↓ 网络传输                  
                               TCP: 接收[加密字节]
                                ↓ 
                               SSL: 解密 → "HTTP/1.1 200 OK"
                                ↓
                               应用: 处理HTTP响应
*/