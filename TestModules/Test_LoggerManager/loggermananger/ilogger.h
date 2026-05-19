#ifdef _WIN32
// 排除 RPC 定义，避免与 std::byte 冲突
#define NO_RPC
// 减少 windows.h 的内容，避免更多冲突
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#endif

#ifndef ILOGGER_H
#define ILOGGER_H

#include <string>
#include "loggermanager.h"

/**
 * @brief 日志接口类，对 LoggerManager 的轻量封装
 *
 * 绑定一个日志文件路径，提供无需重复指定路径和级别的简化写入接口。
 * 典型用法：每个模块持有一个 ILogger 实例，各自写入独立的日志文件。
 *
 * 示例：
 *   ILogger logger("network");
 *   logger.info("连接成功，IP={}", "192.168.1.1");
 *   logger.error("连接超时");
 *   logger.flush();
 */
class ILogger
{
public:
    /**
     * @brief 构造函数，绑定日志文件名
     * @param log_file 日志文件名（无需 .log 后缀，自动补全）
     */
    explicit ILogger(const std::string& log_file = "default")
        : log_file_(log_file)
    {}

    ILogger(const ILogger&) = delete;
    ILogger& operator=(const ILogger&) = delete;

    ILogger(ILogger&& other) noexcept
        : log_file_(std::move(other.log_file_))
    {}

    ILogger& operator=(ILogger&& other) noexcept
    {
        if (this != &other) {
            log_file_ = std::move(other.log_file_);
        }
        return *this;
    }

    /**
     * @brief 设置当前日志文件名
     * @param log_file 日志文件名（无需 .log 后缀）
     */
    void set_log_file(const std::string& log_file)
    {
        log_file_ = log_file;
    }

    /**
     * @brief 获取当前日志文件名
     */
    std::string get_log_file() const
    {
        return log_file_;
    }

    /**
     * @brief 写入 DEBUG 级别日志
     * @param fmt 格式化字符串，使用 {} 作为占位符
     * @param args 格式化参数
     */
    template<typename... Args>
    void debug(const std::string& fmt, Args&&... args)
    {
        LoggerManager::getInstance()->log(log_file_, Level::DEBUG, fmt, std::forward<Args>(args)...);
    }

    /**
     * @brief 写入 INFO 级别日志
     * @param fmt 格式化字符串，使用 {} 作为占位符
     * @param args 格式化参数
     */
    template<typename... Args>
    void info(const std::string& fmt, Args&&... args)
    {
        LoggerManager::getInstance()->log(log_file_, Level::INFO, fmt, std::forward<Args>(args)...);
    }

    /**
     * @brief 写入 WARN 级别日志
     * @param fmt 格式化字符串，使用 {} 作为占位符
     * @param args 格式化参数
     */
    template<typename... Args>
    void warn(const std::string& fmt, Args&&... args)
    {
        LoggerManager::getInstance()->log(log_file_, Level::WARN, fmt, std::forward<Args>(args)...);
    }

    /**
     * @brief 写入 ERROR 级别日志
     * @param fmt 格式化字符串，使用 {} 作为占位符
     * @param args 格式化参数
     */
    template<typename... Args>
    void error(const std::string& fmt, Args&&... args)
    {
        LoggerManager::getInstance()->log(log_file_, Level::ERROR, fmt, std::forward<Args>(args)...);
    }

    /**
     * @brief 写入 TRACE 级别日志
     * @param fmt 格式化字符串，使用 {} 作为占位符
     * @param args 格式化参数
     */
    template<typename... Args>
    void trace(const std::string& fmt, Args&&... args)
    {
        LoggerManager::getInstance()->log(log_file_, Level::TRACE, fmt, std::forward<Args>(args)...);
    }

    /**
     * @brief 写入 CRITICAL 级别日志
     * @param fmt 格式化字符串，使用 {} 作为占位符
     * @param args 格式化参数
     */
    template<typename... Args>
    void critical(const std::string& fmt, Args&&... args)
    {
        LoggerManager::getInstance()->log(log_file_, Level::CRITICAL, fmt, std::forward<Args>(args)...);
    }

    /**
     * @brief 手动刷新当前日志文件，立即将缓冲区写入磁盘
     */
    void flush()
    {
        LoggerManager::getInstance()->flush(log_file_);
    }

private:
    std::string log_file_;  // 绑定的日志文件名
};

#endif // ILOGGER_H
