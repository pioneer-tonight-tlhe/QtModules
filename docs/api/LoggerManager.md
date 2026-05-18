# LoggerManager API 文档

## 概述

`LoggerManager` 是一个基于 spdlog 的高性能线程安全日志管理器，支持异步日志写入、日志回滚、自动清理等功能。该类继承自通用 `Singleton<T>` 模板（CRTP 模式），采用基于 `std::call_once` 的线程安全单例实现，适用于高并发场景。

继承关系：

```cpp
class LoggerManager : public Singleton<LoggerManager>
```

详见 [Singleton 模板使用文档](./Singleton.md)。

## 主要特性

- **异步日志**：使用 spdlog 异步线程池，支持高并发写入
- **日志回滚**：支持按文件大小自动回滚，可配置最大文件大小和保留文件数量
- **自动清理**：支持按天数自动清理过期日志目录
- **多级日志**：支持 TRACE、DEBUG、INFO、WARN、ERROR、CRITICAL 六个日志级别
- **自动汇总**：支持自动将所有日志汇总到 trace 日志文件
- **错误分离**：支持自动将 WARN 和 ERROR 级别日志分离到独立文件
- **跨平台**：支持 Windows 和 Linux/macOS 平台

## 日志级别

```cpp
enum class Level {
    TRACE,    // 跟踪信息
    DEBUG,    // 调试信息
    INFO,     // 一般信息
    WARN,     // 警告信息
    ERROR,    // 错误信息
    CRITICAL  // 严重错误
};
```

## 公共 API

### 单例获取

```cpp
static std::shared_ptr<LoggerManager> getInstance();
```

**说明**：获取 LoggerManager 单例实例。该方法由 `Singleton<LoggerManager>` 基类提供，基于 `std::call_once` 保证线程安全。

**返回值**：`std::shared_ptr<LoggerManager>` - 单例实例的智能指针

**示例**：
```cpp
auto logger = LoggerManager::getInstance();
logger->set_root_dir("./logs");
```

---

### 设置根目录

```cpp
void set_root_dir(const std::string& root_dir);
```

**说明**：设置日志存储的根目录。日志文件将按日期存储在该目录下的子目录中。

**参数**：
- `root_dir`：根目录路径（默认：`"./logs"`）

**示例**：
```cpp
LoggerManager::getInstance()->set_root_dir("./my_logs");
```

---

### 设置日志回滚参数

```cpp
void set_rotation(size_t max_file_size = 10 * 1024 * 1024, size_t max_files = 5);
```

**说明**：设置日志文件回滚参数。当日志文件达到指定大小时，会自动创建新文件并保留指定数量的历史文件。

**参数**：
- `max_file_size`：单个日志文件最大大小（默认：10MB）
- `max_files`：保留的历史文件数量（默认：5）

**示例**：
```cpp
// 设置单个文件最大 20MB，保留 10 个历史文件
LoggerManager::getInstance()->set_rotation(20 * 1024 * 1024, 10);
```

---

### 设置日志保留天数

```cpp
void set_retention_days(int days);
```

**说明**：设置日志保留天数。超过指定天数的日志目录将被自动清理。

**参数**：
- `days`：保留天数（默认：7 天）

**示例**：
```cpp
// 保留 30 天的日志
LoggerManager::getInstance()->set_retention_days(30);
```

---

### 设置 trace 日志文件名

```cpp
void set_trace_filename(const std::string& name);
```

**说明**：设置自动 trace 日志的文件名。启用自动 trace 后，所有日志将汇总到此文件。

**参数**：
- `name`：trace 日志文件名（默认：`"trace.log"`）

**示例**：
```cpp
LoggerManager::getInstance()->set_trace_filename("all.log");
```

---

### 启用自动 trace 日志

```cpp
void enable_auto_trace(bool enabled);
```

**说明**：启用或禁用自动 trace 功能。启用后，所有日志都会同时写入到 trace 日志文件中。

**参数**：
- `enabled`：是否启用（默认：false）

**示例**：
```cpp
LoggerManager::getInstance()->enable_auto_trace(true);
```

---

### 启用 WARN/ERROR 分离

```cpp
void enable_warn_error_split(bool enabled);
```

**说明**：启用或禁用 WARN 和 ERROR 日志分离功能。启用后，WARN 级别日志会写入 `*_warn.log`，ERROR 级别日志会写入 `*_error.log`。

**参数**：
- `enabled`：是否启用（默认：false）

**示例**：
```cpp
LoggerManager::getInstance()->enable_warn_error_split(true);
```

---

### 清理日志系统

```cpp
void shutdown();
```

**说明**：清理日志系统，刷新所有日志到磁盘，释放资源。通常在程序退出前调用。

**示例**：
```cpp
LoggerManager::getInstance()->shutdown();
```

---

### 手动刷新日志

```cpp
void flush(const std::string& file_name);
```

**说明**：手动刷新指定日志文件的缓冲区，立即写入磁盘。

**参数**：
- `file_name`：日志文件名（会自动添加 `.log` 后缀）

**示例**：
```cpp
LoggerManager::getInstance()->flush("debug");
```

---

### 核心日志方法

```cpp
template<typename... Args>
void log(const std::string& file_name, Level level,
         const std::string& fmt, Args&&... args);
```

**说明**：写入日志。支持格式化字符串，使用 `{}` 作为占位符。

**参数**：
- `file_name`：日志文件名（会自动添加 `.log` 后缀）
- `level`：日志级别
- `fmt`：格式化字符串
- `args...`：格式化参数

**示例**：
```cpp
LoggerManager::getInstance()->log("debug", Level::INFO, "hello world {} {}!", "simon", 12);
```

---

### 设置当前日志文件名

```cpp
void set_current_log_file(const std::string& file_name);
```

**说明**：设置当前日志文件名，配合 `debug()`、`info()`、`warn()`、`error()` 便捷方法使用。设置后，便捷方法将自动写入到该文件。该值存储在内部成员变量中，线程安全。

**参数**：
- `file_name`：日志文件名（会自动添加 `.log` 后缀，默认：`"default.log"`）

**示例**：
```cpp
LoggerManager::getInstance()->set_current_log_file("app");
```

---

### 获取当前日志文件名

```cpp
std::string get_current_log_file();
```

**说明**：获取当前设置的日志文件名（已规范化，包含 `.log` 后缀）。

**返回值**：当前日志文件名字符串

**示例**：
```cpp
std::string current = LoggerManager::getInstance()->get_current_log_file();
// 输出: "app.log"
```

---

### 便捷日志方法（debug / info / warn / error）

```cpp
template<typename... Args> void debug(const std::string& fmt, Args&&... args);
template<typename... Args> void info (const std::string& fmt, Args&&... args);
template<typename... Args> void warn (const std::string& fmt, Args&&... args);
template<typename... Args> void error(const std::string& fmt, Args&&... args);
```

**说明**：四种常用级别的便捷写日志方法，**不需要传入文件名和级别参数**。这些方法会使用 `set_current_log_file()` 设置的当前日志文件名作为目标文件。

**参数**：
- `fmt`：格式化字符串（使用 `{}` 占位符）
- `args...`：格式化参数

**前置条件**：建议先调用 `set_current_log_file()` 设置当前日志文件名；如果未设置，则默认写入 `default.log`。

**示例**：
```cpp
auto logger = LoggerManager::getInstance();
logger->set_current_log_file("app");          // 先设置当前日志文件

logger->debug("调试信息：{}", "some value");   // 写入 app.log（DEBUG）
logger->info("程序启动");                      // 写入 app.log（INFO）
logger->warn("警告：{}", "磁盘空间不足");      // 写入 app.log（WARN）
logger->error("错误：{}", "文件打开失败");     // 写入 app.log（ERROR）
```

---

## 使用示例

### 基础使用（核心 log 方法）

```cpp
#include "loggermanager.h"

int main() {
    // 获取单例
    auto logger = LoggerManager::getInstance();
    
    // 写入日志（每次都需指定文件名和级别）
    logger->log("debug", Level::INFO, "程序启动");
    logger->log("debug", Level::DEBUG, "调试信息：{}", "some value");
    logger->log("debug", Level::WARN, "警告：{}", "磁盘空间不足");
    logger->log("debug", Level::ERROR, "错误：{}", "文件打开失败");
    
    // 清理
    logger->shutdown();
    
    return 0;
}
```

### 便捷方法使用（推荐）

```cpp
#include "loggermanager.h"

int main() {
    // 获取单例
    auto logger = LoggerManager::getInstance();
    
    // 设置当前日志文件名（仅需设置一次）
    logger->set_current_log_file("app");
    
    // 使用便捷方法写日志，无需重复传文件名和级别
    logger->info("程序启动");
    logger->debug("调试信息：{}", "some value");
    logger->warn("警告：{}", "磁盘空间不足");
    logger->error("错误：{}", "文件打开失败");
    
    // 切换到其他日志文件
    logger->set_current_log_file("network");
    logger->info("连接成功，IP={}", "192.168.1.1");
    
    // 获取当前日志文件名
    std::string current = logger->get_current_log_file();  // "network.log"
    
    // 清理
    logger->shutdown();
    
    return 0;
}
```

### 完整配置示例

```cpp
#include "loggermanager.h"

int main() {
    // 获取单例
    auto logger = LoggerManager::getInstance();
    
    // 配置日志系统
    logger->set_root_dir("./logs");              // 设置根目录
    logger->set_rotation(20 * 1024 * 1024, 10);  // 单文件 20MB，保留 10 个
    logger->set_retention_days(30);              // 保留 30 天
    logger->set_trace_filename("all.log");       // trace 文件名
    
    // 启用自动 trace 和错误分离
    logger->enable_auto_trace(true);
    logger->enable_warn_error_split(true);
    
    // 写入日志
    logger->log("app", Level::INFO, "应用启动");
    logger->log("app", Level::DEBUG, "用户 ID：{}", 12345);
    logger->log("app", Level::WARN, "API 响应慢：{}ms", 500);
    logger->log("app", Level::ERROR, "数据库连接失败");
    
    // 手动刷新
    logger->flush("app");
    
    // 清理
    logger->shutdown();
    
    return 0;
}
```

### 高并发场景示例

```cpp
#include "loggermanager.h"
#include <thread>
#include <vector>

void worker(int id) {
    auto logger = LoggerManager::getInstance();
    for (int i = 0; i < 1000; ++i) {
        logger->log("worker", Level::INFO, "Worker {} - 消息 {}", id, i);
    }
}

int main() {
    auto logger = LoggerManager::getInstance();
    
    // 配置适合高并发场景
    logger->set_rotation(50 * 1024 * 1024, 20);  // 50MB，保留 20 个
    logger->enable_auto_trace(true);
    
    // 创建多个线程并发写入日志
    std::vector<std::thread> threads;
    for (int i = 0; i < 100; ++i) {
        threads.emplace_back(worker, i);
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
    
    logger->shutdown();
    
    return 0;
}
```

## 日志文件结构

启用所有功能后，日志文件结构如下：

```
logs/
├── 2026-05-13/               # 日期目录
│   ├── app.log              # 主日志文件
│   ├── app_warn.log         # WARN 级别日志
│   ├── app_error.log        # ERROR 线别日志
│   ├── all.log              # 汇总 trace 日志
│   └── debug.log            # 其他日志文件
├── 2026-05-12/               # 前一天的日志
└── 2026-05-11/               # 更早的日志
```

## 性能特性

- **异步写入**：使用 spdlog 异步线程池，队列容量 131072，2 个 I/O 线程
- **批量优化**：支持批量获取 logger，减少锁获取次数
- **读写锁**：使用 `std::shared_mutex` 支持多读单写
- **缓存机制**：logger 缓存和目录缓存，避免重复创建
- **日期检测**：自动检测日期变化，清理旧 logger 缓存

## 线程安全

所有公共 API 都是线程安全的，可以在多线程环境中安全使用。

## 依赖

- spdlog (异步日志库)
- C++11 或更高版本

## 注意事项

1. 程序退出前务必调用 `shutdown()` 以确保所有日志写入磁盘
2. 日志文件名会自动添加 `.log` 后缀，无需手动添加
3. 日志目录会自动创建，无需手动创建
4. 清理线程每 24 小时自动清理一次过期日志
