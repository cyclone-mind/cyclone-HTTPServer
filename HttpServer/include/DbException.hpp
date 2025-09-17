/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-17 12:39:09
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 12:39:12
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/DbException.hpp
 * @Description: 
 * Copyright (c) 2025 by cyclone, All Rights Reserved. 
 */
#pragma once
#include <stdexcept>
#include <string>

namespace http::db {

class DbException : public std::runtime_error 
{
public:
    explicit DbException(const std::string& message) 
        : std::runtime_error(message) {}
    
    explicit DbException(const char* message) 
        : std::runtime_error(message) {}
};
}