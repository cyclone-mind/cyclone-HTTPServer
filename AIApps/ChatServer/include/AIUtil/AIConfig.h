#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../../../../HttpServer/include/JsonUtil.h"  


/**
 * AITool 结构体
 * 表示 AI 可以调用的工具定义
 */
struct AITool {
    std::string name;   // 工具名称
    std::unordered_map<std::string, std::string> params;  // 参数定义（参数名 -> 参数描述）
    std::string desc;   // 工具描述
};


/**
 * AIToolCall 结构体
 * 表示 AI 的工具调用请求
 */
struct AIToolCall {
    std::string toolName;  // 要调用的工具名称
    json args;             // 工具调用参数（JSON格式）
    bool isToolCall = false;  // 标识是否为工具调用
};


/**
 * AIConfig 类
 * AI 配置管理类
 *
 * 功能：
 * - 从配置文件加载 AI 的提示词模板和可用工具列表
 * - 根据用户输入构建完整的提示词
 * - 解析 AI 的响应，识别工具调用
 * - 构建工具调用结果的提示词
 */
class AIConfig {
public:
    /**
     * 从配置文件加载 AI 配置
     * @param path 配置文件路径（通常是JSON格式）
     * @return 加载成功返回true，失败返回false
     */
    bool loadFromFile(const std::string& path);

    /**
     * 根据用户输入构建完整提示词
     * @param userInput 用户的原始输入
     * @return 包含工具列表和用户输入的完整提示词
     */
    std::string buildPrompt(const std::string& userInput) const;

    /**
     * 解析 AI 的响应，检测是否包含工具调用
     * @param response AI 的原始响应文本
     * @return AIToolCall 对象，包含工具调用信息
     */
    AIToolCall parseAIResponse(const std::string& response) const;

    /**
     * 构建工具调用结果的提示词
     * @param userInput 用户的原始输入
     * @param toolName 被调用的工具名称
     * @param toolArgs 工具调用参数
     * @param toolResult 工具执行结果
     * @return 包含工具执行结果的提示词
     */
    std::string buildToolResultPrompt(const std::string& userInput,const std::string& toolName,const json& toolArgs,const json& toolResult) const;

private:
    std::string promptTemplate_;   // 提示词模板
    std::vector<AITool> tools_;    // 可用工具列表

    /**
     * 构建工具列表的文本描述
     * @return 工具列表的格式化字符串
     */
    std::string buildToolList() const;
};
