#pragma once
#include <chrono>
#include <random>
#include <cstdlib>
#include <ctime>
#include <string>


/**
 * AISessionIdGenerator 类
 * AI 会话ID生成器
 *
 * 功能：
 * - 为每个新的 AI 对话会话生成唯一的 session_id
 * - 使用时间戳和随机数确保ID的唯一性
 *
 * 用途：
 * - 支持同一用户创建多个独立的对话会话
 * - 每个会话有独立的对话上下文和历史记录
 */
class AISessionIdGenerator {
public:
    /**
     * 构造函数
     * 初始化随机数种子（使用当前时间）
     */
    AISessionIdGenerator() {
        // 使用当前时间作为随机数种子
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

    /**
     * 生成一个新的会话ID
     * @return 唯一的会话ID字符串
     */
    std::string generate();
};
