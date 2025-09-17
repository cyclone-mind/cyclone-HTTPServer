/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-17 17:23:36
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 20:32:20
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/SslContext.hpp
 * @Description: 
 * Copyright (c) 2025 by cyclone, All Rights Reserved. 
 */
#pragma once
#include <openssl/ssl.h>
#include <memory>
#include <muduo/base/noncopyable.h>
#include "SslConfig.hpp"
#include "muduo/base/Logging.h"

namespace ssl {
    class SslContext {
        private:
        SSL_CTX* ctx_;
        SslConfig config_; // SSL配置
        public:
        SslContext(const SslConfig& config) : ctx_(nullptr),config_(config){}
        auto initialize() -> bool;
        private:
        void static handleSslError(const std::string& msg);
        // 给ssl上下文设置证书、私钥，验证私钥，设置证书链
        auto loadCertificates() -> bool;
        // 给ssl上下文设置 ssl 协议版本
        auto setupProtocol() -> bool;
        
    };
}