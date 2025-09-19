/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-18 10:44:11
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-18 15:45:23
 * @FilePath: /cyclone-HTTPServer/HttpServer/src/SslConnection.cpp
 * @Description:
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#include "../include/SslConnection.hpp"
#include <openssl/err.h>

namespace ssl {
SslConnection::SslConnection(const TcpConnectionPtr& conn, SslContext* ctx)
    : ssl_(nullptr),
      ctx_(ctx),
      conn_(conn),
      state_(SSLState::HANDSHAKE),
      readBio_(nullptr),
      writeBio_(nullptr),
      messageCallback_(nullptr) {
    // 创建 SSL 对象
    ssl_ = SSL_new(ctx_->getNativeHandle());
    if (ssl_ == nullptr) {
        LOG_ERROR << "Failed to create SSL object: " << ERR_error_string(ERR_get_error(), nullptr);
        return;
    }

    // 创建 BIO，BIO 是 OpenSSL 中的抽象I/O接口
    readBio_ = BIO_new(BIO_s_mem()); // 使用标准内存BIO
    writeBio_ = BIO_new(BIO_s_mem()); // 使用标准内存BIO

    if ((readBio_ == nullptr) || (writeBio_ == nullptr)) {
        LOG_ERROR << "Failed to create BIO objects";
        SSL_free(ssl_);
        ssl_ = nullptr;
        return;
    }

    SSL_set_bio(ssl_, readBio_, writeBio_);
    SSL_set_accept_state(ssl_);  // 设置为服务器模式

    // 设置 SSL 选项
    SSL_set_mode(ssl_, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
    SSL_set_mode(ssl_, SSL_MODE_ENABLE_PARTIAL_WRITE);

    // 设置连接回调
    conn_->setMessageCallback([this](auto&& PH1, auto&& PH2, auto&& PH3) {
        onRead(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2),
               std::forward<decltype(PH3)>(PH3));
    });

}
SslConnection::~SslConnection() 
{
    if (ssl_ != nullptr) 
    {
        SSL_free(ssl_);  // 这会同时释放 BIO
        ssl_ = nullptr;
    }
}

void SslConnection::onRead(const TcpConnectionPtr& conn, BufferPtr buf, 
                         muduo::Timestamp time) 
{
    if (state_ == SSLState::HANDSHAKE) {
        // ssl_调用握手时知道如何处理 readBio_ 里面的数据，我们只需要将数据写入 readBio_
        // 下面三行 处理某一轮的握手数据，也就是单次握手处理。

        // 1. 将本次 onRead 接收到的数据写入 readBio_
        // 这些数据是客户端在当前轮次发送的握手消息
        BIO_write(readBio_, buf->peek(), buf->readableBytes());
        // 2. 清空网络缓冲区 标记这些数据已被处理，
        buf->retrieve(buf->readableBytes());
        // 3. 尝试推进握手状态机，可能成功完成握手，也可能返回 SSL_ERROR_WANT_READ
        handleHandshake();

        return;
    } else if (state_ == SSLState::ESTABLISHED) {
        // 先将接收到的数据写入SSL的BIO
        BIO_write(readBio_, buf->peek(), buf->readableBytes());
        buf->retrieve(buf->readableBytes());

        // 循环解密所有数据
        muduo::net::Buffer decryptedBuffer;
        char decryptedData[4096];
        int ret;
        
        // 循环读取直到没有更多解密数据
        while ((ret = SSL_read(ssl_, decryptedData, sizeof(decryptedData))) > 0) {
            decryptedBuffer.append(decryptedData, ret);
        }
        
        // 检查是否有解密的数据需要处理
        if (decryptedBuffer.readableBytes() > 0) {
            // 调用上层回调处理解密后的数据
            if (messageCallback_) {
                messageCallback_(conn, &decryptedBuffer, time);
            }
        } else {
            // 处理 SSL 错误（如果有）
            if (ret < 0) {
                SSLError error = getLastError(ret);
                if (error != SSLError::WANT_READ) {
                    handleError(error);
                }
            }
        }
    }
}

void SslConnection::send(const void* data, size_t len) 
{
    if (state_ != SSLState::ESTABLISHED) {
        LOG_ERROR << "Cannot send data before SSL handshake is complete";
        return;
    }
    
    // SSL 加密明文，存储到 writeBio_
    int written = SSL_write(ssl_, data, len);
    if (written <= 0) {
        SSLError error = getLastError(written);
        handleError(error);
        return;
    }
    
    sendPendingData();
}

void SslConnection::sendPendingData() 
{
    // 从writeBio_读取加密后的数据并发送
    int pending;
    while ((pending = BIO_pending(writeBio_)) > 0) {   // 检查有多少加密数据待发送
        LOG_INFO << "Sending " << pending << " bytes of SSL data to client";
        
        // 根据待发送数据大小分配缓冲区，但限制最大值
        size_t bufSize = std::min(static_cast<size_t>(pending), static_cast<size_t>(16384)); // 最大16KB
        std::vector<char> buf(bufSize);

        // 从 writeBio_ 读取加密数据
        int bytes = BIO_read(writeBio_, buf.data(), bufSize);
        if (bytes > 0) {
            // 发送到TCP连接
            conn_->send(buf.data(), bytes);
            LOG_INFO << "Actually sent " << bytes << " bytes";
        }
    }
}

void SslConnection::startHandshake() 
{
    // 设置为服务器模式
    SSL_set_accept_state(ssl_);
    handleHandshake();
}

void SslConnection::handleHandshake() 
{
    LOG_INFO << "Starting SSL handshake";
    
    // 执行 SSL 握手操作
    int ret = SSL_do_handshake(ssl_);
    
    LOG_INFO << "SSL_do_handshake returned: " << ret;
    
    // 检查握手是否成功完成
    if (ret == 1) {
        // 更新连接状态为已建立
        state_ = SSLState::ESTABLISHED;
        LOG_INFO << "SSL handshake completed successfully";
        // 记录使用的加密套件
        LOG_INFO << "Using cipher: " << SSL_get_cipher(ssl_);
        // 记录 SSL 协议版本
        LOG_INFO << "Protocol version: " << SSL_get_version(ssl_);
        
        // 握手完成后，确保设置了正确的回调
        if (!messageCallback_) {
            LOG_WARN << "No message callback set after SSL handshake";
        }
        return;
    }
    
    // 获取 SSL 错误码以判断握手状态
    int err = SSL_get_error(ssl_, ret);
    LOG_INFO << "SSL error code: " << err;
    
    switch (err) {
        case SSL_ERROR_WANT_READ:
            LOG_INFO << "SSL handshake needs more data (WANT_READ)";
            // 检查是否有数据需要发送给客户端
            sendPendingData();
            break;
        case SSL_ERROR_WANT_WRITE:
            LOG_INFO << "SSL handshake needs to write (WANT_WRITE)";
            // 发送待发送的数据
            sendPendingData();
            break;
            
        default: {
            // 获取详细的错误信息
            char errBuf[256];
            // 从错误队列中获取错误码
            unsigned long errCode = ERR_get_error();
            // 将错误码转换为可读的错误信息
            ERR_error_string_n(errCode, errBuf, sizeof(errBuf));
            LOG_ERROR << "SSL handshake failed: " << errBuf;
            conn_->shutdown();  // 关闭连接
            break;
        }
    }
    
    // 无论何种情况，都检查是否有数据需要发送
    sendPendingData();
}

auto SslConnection::getLastError(int ret) -> SSLError 
{
    int err = SSL_get_error(ssl_, ret);
    switch (err) 
    {
        case SSL_ERROR_NONE:
            return SSLError::NONE;
        case SSL_ERROR_WANT_READ:
            return SSLError::WANT_READ;
        case SSL_ERROR_WANT_WRITE:
            return SSLError::WANT_WRITE;
        case SSL_ERROR_SYSCALL:
            return SSLError::SYSCALL;
        case SSL_ERROR_SSL:
            return SSLError::SSL;
        default:
            return SSLError::UNKNOWN;
    }
}

void SslConnection::handleError(SSLError error) 
{
    switch (error) 
    {
        case SSLError::WANT_READ:
        case SSLError::WANT_WRITE:
            // 需要等待更多数据或写入缓冲区可用
            break;
        case SSLError::SSL:
        case SSLError::SYSCALL:
        case SSLError::UNKNOWN:
            LOG_ERROR << "SSL error occurred: " << ERR_error_string(ERR_get_error(), nullptr);
            state_ = SSLState::ERROR;
            conn_->shutdown();
            break;
        default:
            break;
    }
}

}  // namespace ssl