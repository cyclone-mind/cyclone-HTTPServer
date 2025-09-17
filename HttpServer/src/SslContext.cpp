/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-17 17:24:58
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 20:32:03
 * @FilePath: /cyclone-HTTPServer/HttpServer/src/SslContext.cpp
 * @Description:
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#include "../include/SslContext.hpp"

#include <openssl/err.h>

namespace ssl {
auto SslContext::initialize() -> bool {
    // 初始化 OpenSSL
    OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, nullptr);

    // 创建 SSL 上下文
    const SSL_METHOD* method = TLS_server_method();
    ctx_ = SSL_CTX_new(method);
    if (!ctx_) {
        handleSslError("Failed to create SSL context");
        return false;
    }

    // 设置 SSL 选项
    long options =
        SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION | SSL_OP_CIPHER_SERVER_PREFERENCE;
    SSL_CTX_set_options(ctx_, options);

    // 加载证书和私钥
    if (!loadCertificates()) {
        return false;
    }

    // 设置协议版本
    if (!setupProtocol()) {
        return false;
    }

    // 设置会话缓存
    // 设置会话缓存模式、缓存大小和超时时间
    SSL_CTX_set_session_cache_mode(ctx_, SSL_SESS_CACHE_SERVER);
    SSL_CTX_sess_set_cache_size(ctx_, config_.getSessionCacheSize());
    SSL_CTX_set_timeout(ctx_, config_.getSessionTimeout());

    LOG_INFO << "SSL context initialized successfully";
    return true;
}
void SslContext::handleSslError(const std::string& msg) {
    // 256字节足够存储大多数OpenSSL错误信息
    std::array<char, 256> buf;

    // 使用ERR_error_string_n确保缓冲区不会溢出
    // 这是线程安全的版本，比ERR_error_string更安全
    // ERR_get_error()获取最新的OpenSSL错误码并清除错误队列
    ERR_error_string_n(ERR_get_error(), buf.data(), buf.size());

    LOG_ERROR << msg << ": " << buf.data();
}
auto SslContext::loadCertificates() -> bool {
    // 加载证书
    if (SSL_CTX_use_certificate_file(ctx_, config_.getCertificateFile().c_str(),
                                     SSL_FILETYPE_PEM) <= 0) {
        handleSslError("Failed to load server certificate");
        return false;
    }

    // 加载私钥
    if (SSL_CTX_use_PrivateKey_file(ctx_, config_.getPrivateKeyFile().c_str(), SSL_FILETYPE_PEM) <=
        0) {
        handleSslError("Failed to load private key");
        return false;
    }

    // 验证私钥
    if (SSL_CTX_check_private_key(ctx_) == 0) {
        handleSslError("Private key does not match the certificate");
        return false;
    }

    // 加载证书链
    if (!config_.getCertificateChainFile().empty()) {
        if (SSL_CTX_use_certificate_chain_file(ctx_, config_.getCertificateChainFile().c_str()) <=
            0) {
            handleSslError("Failed to load certificate chain");
            return false;
        }
    }

    return true;
}

auto SslContext::setupProtocol() -> bool {
    // 设置 SSL/TLS 协议版本
    long options = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3;
    switch (config_.getProtocolVersion()) {
        case SSLVersion::TLS_1_0:
            options |= SSL_OP_NO_TLSv1;
            break;
        case SSLVersion::TLS_1_1:
            options |= SSL_OP_NO_TLSv1_1;
            break;
        case SSLVersion::TLS_1_2:
            options |= SSL_OP_NO_TLSv1_2;
            break;
        case SSLVersion::TLS_1_3:
            options |= SSL_OP_NO_TLSv1_3;
            break;
    }
    SSL_CTX_set_options(ctx_, options);

    // 设置加密套件
    if (!config_.getCipherList().empty()) {
        if (SSL_CTX_set_cipher_list(ctx_, config_.getCipherList().c_str()) <= 0) {
            handleSslError("Failed to set cipher list");
            return false;
        }
    }

    return true;
}
}  // namespace ssl