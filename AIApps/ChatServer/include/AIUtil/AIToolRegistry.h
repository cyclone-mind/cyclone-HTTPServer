#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <ctime>
#include <curl/curl.h>

#include "../../../../HttpServer/include/JsonUtil.h"

/**
 * AIToolRegistry 类
 * AI 工具注册表
 *
 * 功能：
 * - 管理 AI 可以调用的所有工具（函数）
 * - 支持动态注册和调用工具
 * - 内置常用工具（天气查询、时间查询等）
 *
 * 设计模式：注册表模式
 * - 工具以名称-函数对的形式存储
 * - AI 可以通过工具名称调用对应的函数
 * - 便于扩展新的工具功能
 */
class AIToolRegistry {
public:
    // 工具函数类型定义（输入JSON参数，返回JSON结果）
    using ToolFunc = std::function<json(const json&)>;

    /**
     * 构造函数
     * 自动注册内置工具
     */
    AIToolRegistry();

    /**
     * 注册一个新的工具
     * @param name 工具名称
     * @param func 工具函数（接收JSON参数，返回JSON结果）
     */
    void registerTool(const std::string& name, ToolFunc func);

    /**
     * 调用指定的工具
     * @param name 工具名称
     * @param args 工具参数（JSON格式）
     * @return 工具执行结果（JSON格式）
     * @throws runtime_error 如果工具不存在
     */
    json invoke(const std::string& name, const json& args) const;

    /**
     * 检查工具是否存在
     * @param name 工具名称
     * @return 存在返回true，否则返回false
     */
    bool hasTool(const std::string& name) const;

private:
    std::unordered_map<std::string, ToolFunc> tools_;  // 工具映射表

    // === 内置工具函数 ===

    /**
     * curl 回调函数，用于接收HTTP响应数据
     */
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

    /**
     * 获取天气信息工具
     * @param args 参数（包含城市名等）
     * @return 天气信息（JSON格式）
     */
    static json getWeather(const json& args);

    /**
     * 获取当前时间工具
     * @param args 参数（可选的时区等）
     * @return 当前时间（JSON格式）
     */
    static json getTime(const json& args);
};
