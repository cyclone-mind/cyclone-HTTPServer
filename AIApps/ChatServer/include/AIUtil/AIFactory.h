#pragma once
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <sstream>
#include <memory>
#include <functional>
#include <unordered_map>
#include <string>


#include "./AIStrategy.h"

/**
 * StrategyFactory 类
 * AI 策略工厂（单例模式 + 工厂模式）
 *
 * 功能：
 * - 管理所有可用的 AI 策略（AliyunStrategy, DouBaoStrategy 等）
 * - 通过名称创建对应的策略实例
 * - 支持动态注册新的策略类型
 *
 * 设计模式：
 * - 单例模式：确保全局只有一个工厂实例
 * - 工厂模式：根据策略名称创建对应的策略对象
 */
class StrategyFactory {

public:
    // 策略创建器类型定义（函数对象，返回策略智能指针）
    using Creator = std::function<std::shared_ptr<AIStrategy>()>;

    /**
     * 获取工厂单例实例
     * @return 工厂实例引用
     */
    static StrategyFactory& instance();

    /**
     * 注册一个新的策略类型
     * @param name 策略名称（如 "aliyun", "doubao"）
     * @param creator 创建该策略的函数对象
     */
    void registerStrategy(const std::string& name, Creator creator);

    /**
     * 根据名称创建策略实例
     * @param name 策略名称
     * @return 策略智能指针，如果找不到则返回空指针
     */
    std::shared_ptr<AIStrategy> create(const std::string& name);

private:
    StrategyFactory() = default;  // 私有构造函数（单例模式）
    std::unordered_map<std::string, Creator> creators;  // 策略创建器映射表
};





/**
 * StrategyRegister 模板类
 * 策略自动注册器
 *
 * 功能：
 * - 在编译时自动将策略类注册到工厂
 * - 通过全局对象的构造函数实现自动注册
 *
 * 使用示例：
 * static StrategyRegister<AliyunStrategy> aliyun("aliyun");
 * 这样在程序启动时会自动注册 AliyunStrategy
 */
template<typename T>
struct StrategyRegister {
    /**
     * 构造函数，自动注册策略
     * @param name 策略名称
     */
    StrategyRegister(const std::string& name) {
        StrategyFactory::instance().registerStrategy(name, [] {
            std::shared_ptr<AIStrategy> instance = std::make_shared<T>();
            return instance;
            });
    }
};

