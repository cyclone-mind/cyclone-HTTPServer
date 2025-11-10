#pragma once
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <sstream>
#include <memory>

#include "../../../../HttpServer/include/JsonUtil.h"

/**
 * AIStrategy 基类
 *
 * 策略模式（Strategy Pattern）的抽象基类
 * 用于支持多种不同的 AI 服务提供商（阿里云、豆包等）
 *
 * 设计模式：策略模式
 * - 每个具体的 AI 提供商实现一个策略类
 * - 通过接口统一不同 AI 服务的调用方式
 * - 便于扩展新的 AI 服务而不修改现有代码
 */
class AIStrategy {
public:
    virtual ~AIStrategy() = default;

    /**
     * 获取 AI 服务的 API URL
     * @return API端点地址
     */
    virtual std::string getApiUrl() const = 0;

    /**
     * 获取 API 密钥
     * @return API Key（通常从环境变量读取）
     */
    virtual std::string getApiKey() const = 0;

    /**
     * 获取使用的模型名称
     * @return 模型名称
     */
    virtual std::string getModel() const = 0;

    /**
     * 构建请求体
     * @param messages 对话历史（内容+时间戳）
     * @return 符合该 AI 服务格式的 JSON 请求体
     */
    virtual json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const = 0;

    /**
     * 解析 AI 服务的响应
     * @param response AI 返回的原始 JSON 响应
     * @return 提取出的回复内容
     */
    virtual std::string parseResponse(const json& response) const = 0;

    // 标识是否为 MCP 模型（Model Context Protocol）
    bool isMCPModel = false;

};

/**
 * AliyunStrategy 类
 * 阿里云通义千问的策略实现
 */
class AliyunStrategy : public AIStrategy {

public:
    /**
     * 构造函数
     * 从环境变量 DASHSCOPE_API_KEY 读取 API 密钥
     * @throws runtime_error 如果环境变量未设置
     */
    AliyunStrategy() {
        const char* key = std::getenv("DASHSCOPE_API_KEY");
        if (!key) throw std::runtime_error("Aliyun API Key not found!");
        apiKey_ = key;
        isMCPModel = false;
    }

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;

private:
    std::string apiKey_;
};

/**
 * DouBaoStrategy 类
 * 字节跳动豆包模型的策略实现
 */
class DouBaoStrategy : public AIStrategy {

public:
    /**
     * 构造函数
     * 从环境变量 DOUBAO_API_KEY 读取 API 密钥
     * @throws runtime_error 如果环境变量未设置
     */
    DouBaoStrategy() {
        const char* key = std::getenv("DOUBAO_API_KEY");
        if (!key) throw std::runtime_error("DOUBAO API Key not found!");
        apiKey_ = key;
        isMCPModel = false;
    }
    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;

private:
    std::string apiKey_;
};

/**
 * AliyunRAGStrategy 类
 * 阿里云 RAG（检索增强生成）模型的策略实现
 */
class AliyunRAGStrategy : public AIStrategy {

public:
    /**
     * 构造函数
     * 从环境变量 DASHSCOPE_API_KEY 读取 API 密钥
     * @throws runtime_error 如果环境变量未设置
     */
    AliyunRAGStrategy() {
        const char* key = std::getenv("DASHSCOPE_API_KEY");
        if (!key) throw std::runtime_error("Aliyun API Key not found!");
        apiKey_ = key;
        isMCPModel = false;
    }

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;

private:
    std::string apiKey_;
};

/**
 * AliyunMcpStrategy 类
 * 阿里云 MCP（Model Context Protocol）模型的策略实现
 */
class AliyunMcpStrategy : public AIStrategy {

public:
    /**
     * 构造函数
     * 从环境变量 DASHSCOPE_API_KEY 读取 API 密钥
     * @throws runtime_error 如果环境变量未设置
     */
    AliyunMcpStrategy() {
        const char* key = std::getenv("DASHSCOPE_API_KEY");
        if (!key) throw std::runtime_error("Aliyun API Key not found!");
        apiKey_ = key;
        isMCPModel = true;  // 这是一个 MCP 模型
    }

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;

private:
    std::string apiKey_;
};







