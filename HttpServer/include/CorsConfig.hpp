/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 16:31:45
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-16 16:32:04
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/CorsConfig.hpp
 * @Description: 
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */
#pragma once

#include <string>
#include <vector>
#define CORS_MAX_AGE 3600

namespace http::middleware 
{

/**
 * @brief CORS (Cross-Origin Resource Sharing) 配置结构体
 * 
 * 设计思想：
 * 1. 提供灵活的CORS配置选项，支持跨域请求的精确控制
 * 2. 采用白名单机制，确保安全性：
 *    - allowedOrigins: 允许的源域名列表，支持通配符"*"
 *    - allowedMethods: 允许的HTTP方法列表
 *    - allowedHeaders: 允许的请求头列表
 * 3. 支持凭证传递控制(allowCredentials)，用于Cookie和认证信息
 * 4. 提供预检请求缓存时间配置(maxAge)，减少预检请求频率
 * 5. 提供默认配置工厂方法，简化常见场景的使用
 */
struct CorsConfig 
{
    /// 允许的源域名列表，"*"表示允许所有域名
    std::vector<std::string> allowedOrigins;
    
    /// 允许的HTTP方法列表
    std::vector<std::string> allowedMethods;
    
    /// 允许的请求头列表
    std::vector<std::string> allowedHeaders;
    
    /// 是否允许发送凭证信息(Cookie、认证头等)
    bool allowCredentials = false;
    
    /// 预检请求结果的缓存时间(秒)
    int maxAge = CORS_MAX_AGE;
    
    /**
     * @brief 创建默认CORS配置
     * @return 包含常用配置的CorsConfig实例
     * 
     * 默认配置说明：
     * - 允许所有域名访问
     * - 支持常见的HTTP方法(GET、POST、PUT、DELETE、OPTIONS)
     * - 允许常见的请求头(Content-Type、Authorization)
     * - 不允许凭证传递(安全考虑)
     */
    static auto defaultConfig() -> CorsConfig 
    {
        CorsConfig config;
        config.allowedOrigins = {"*"};
        config.allowedMethods = {"GET", "POST", "PUT", "DELETE", "OPTIONS"};
        config.allowedHeaders = {"Content-Type", "Authorization","Cookie"};
        return config;
    }

};

} // namespace http