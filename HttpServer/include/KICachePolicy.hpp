/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-19 16:34:05
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-19 19:31:05
 * @FilePath: /cyclone-HTTPServer/HttpServer/include/KICachePolicy.hpp
 * @Description: 
 * Copyright (c) 2025 by cyclone, All Rights Reserved. 
 */
#pragma once

namespace http::cache
{

template <typename Key, typename Value>
class KICachePolicy
{
public:
    virtual ~KICachePolicy() {};

    // 添加缓存接口
    virtual void put(Key key, Value value) = 0;

    // key是传入参数  访问到的值以传出参数的形式返回 | 访问成功返回true
    virtual bool get(Key key, Value& value) = 0;
    // 如果缓存中能找到key，则直接返回value
    virtual Value get(Key key) = 0;

};

} // namespace KamaCache