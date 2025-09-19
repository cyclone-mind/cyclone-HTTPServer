/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-18 16:00:00
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-18 16:12:51
 * @FilePath: /cyclone-HTTPServer/HttpServer/src/SslConfig.cpp
 * @Description:
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#include "../include/SslConfig.hpp"

namespace ssl {

SslConfig::SslConfig()
    : version_{SSLVersion::TLS_1_3},  // 默认使用最新的 TLS 1.3
      cipherList_{"ECDHE+AESGCM:ECDHE+CHACHA20:DHE+AESGCM:DHE+CHACHA20:!aNULL:!MD5:!DSS"},  // 安全的默认加密套件
      verifyClient_{false},  // 默认不验证客户端证书
      verifyDepth_{1},       // 默认验证深度为 1
      sessionTimeout_{300},  // 默认会话超时 5 分钟
      sessionCacheSize_{10240}  // 默认会话缓存大小
{
}

}  // namespace ssl