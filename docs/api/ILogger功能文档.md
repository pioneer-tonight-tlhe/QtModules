# ILogger API 文档

**开发信息**：
- 开发人员：Simon
- 开发时间：2026-05-19

## 概述

`ILogger` 是对 `LoggerManager` 的轻量封装类，通过绑定日志文件名，提供无需重复指定路径和级别的简化写入接口。适用于每个模块各自持有独立日志实例的场景。

**设计目的**：
- 简化日志写入：无需每次传入文件名和级别
- 模块化日志：每个模块持有一个 ILogger 实例，写入独立日志文件
- 语义化接口：使用 `info()`、`error()` 等方法，代码更清晰

## 主要特性

- 绑定日志文件名，自动补全 `.log` 后缀
- 提供 6 个级别的写入方法（trace、debug、info、warn、error、critical）
- 支持格式化字符串，使用 `{}` 占位符
- 禁止拷贝，支持移动语义
- 提供无参 `flush()` 方法

## 日志级别

| 级别 | 枚举值 | 说明 |
|------|--------|------|
| TRACE | `Level::TRACE` | 最详细的跟踪信息 |
| DEBUG | `Level::DEBUG` | 调试信息 |
| INFO | `Level::INFO` | 一般信息 |
| WARN | `Level::WARN` | 警告信息 |
| ERROR | `Level::ERROR` | 错误信息 |
| CRITICAL | `Level::CRITICAL` | 严重错误 |

## 公共 API

### 构造函数

```cpp
explicit ILogger(const std::string& log_file = "default");
```

**说明**：构造 ILogger 实例，绑定日志文件名。

**参数**：
- `log_file`：日志文件名（无需 `.log` 后缀，自动补全），默认为 `"default"`

**示例**：
```cpp
ILogger logger("network");
ILogger defaultLogger;  // 使用默认文件名 "default"
```

---

### 设置日志文件名

```cpp
void set_log_file(const std::string& log_file);
```

**说明**：设置当前绑定的日志文件名。

**参数**：
- `log_file`：日志文件名（无需 `.log` 后缀）

**示例**：
```cpp
ILogger logger("network");
logger.set_log_file("database");  // 切换到 database.log
```

---

### 获取日志文件名

```cpp
std::string get_log_file() const;
```

**说明**：获取当前绑定的日志文件名。

**返回值**：当前日志文件名字符串（不含后缀）

**示例**：
```cpp
ILogger logger("network");
std::string name = logger.get_log_file();  // "network"
```

---

### 写入日志方法

#### trace

```cpp
template<typename... Args>
void trace(const std::string& fmt, Args&&... args);
```

**说明**：写入 TRACE 级别日志。

**参数**：
- `fmt`：格式化字符串，使用 `{}` 占位符
- `args...`：格式化参数

**示例**：
```cpp
logger.trace("函数入口：参数={}", 123);
```

---

#### debug

```cpp
template<typename... Args>
void debug(const std::string& fmt, Args&&... args);
```

**说明**：写入 DEBUG 级别日志。

**参数**：
- `fmt`：格式化字符串，使用 `{}` 占位符
- `args...`：格式化参数

**示例**：
```cpp
logger.debug("调试信息：{}", "some value");
```

---

#### info

```cpp
template<typename... Args>
void info(const std::string& fmt, Args&&... args);
```

**说明**：写入 INFO 级别日志。

**参数**：
- `fmt`：格式化字符串，使用 `{}` 占位符
- `args...`：格式化参数

**示例**：
```cpp
logger.info("连接成功，IP={}", "192.168.1.1");
```

---

#### warn

```cpp
template<typename... Args>
void warn(const std::string& fmt, Args&&... args);
```

**说明**：写入 WARN 级别日志。

**参数**：
- `fmt`：格式化字符串，使用 `{}` 占位符
- `args...`：格式化参数

**示例**：
```cpp
logger.warn("响应较慢：{}ms", 500);
```

---

#### error

```cpp
template<typename... Args>
void error(const std::string& fmt, Args&&... args);
```

**说明**：写入 ERROR 级别日志。

**参数**：
- `fmt`：格式化字符串，使用 `{}` 占位符
- `args...`：格式化参数

**示例**：
```cpp
logger.error("连接超时");
```

---

#### critical

```cpp
template<typename... Args>
void critical(const std::string& fmt, Args&&... args);
```

**说明**：写入 CRITICAL 级别日志。

**参数**：
- `fmt`：格式化字符串，使用 `{}` 占位符
- `args...`：格式化参数

**示例**：
```cpp
logger.critical("系统崩溃，无法恢复");
```

---

### 手动刷新

```cpp
void flush();
```

**说明**：手动刷新当前绑定的日志文件，立即将缓冲区写入磁盘。无需传入文件名。

**示例**：
```cpp
logger.error("重要错误");
logger.flush();  // 立即写入磁盘
```

---

## 使用示例

### 基础使用

```cpp
#include "loggermananger/ilogger.h"

int main() {
    // 初始化 LoggerManager（只需一次）
    LoggerManager::getInstance()->set_root_dir("./logs");

    // 创建 ILogger 实例
    ILogger logger("app");

    // 写入日志
    logger.info("程序启动");
    logger.debug("调试信息：{}", "some value");
    logger.warn("警告：{}", "磁盘空间不足");
    logger.error("错误：{}", "文件打开失败");

    // 手动刷新
    logger.flush();

    return 0;
}
```

### 模块化使用

```cpp
#include "loggermananger/ilogger.h"

// 网络模块
class NetworkModule {
private:
    ILogger logger_;

public:
    NetworkModule() : logger_("network") {}

    void connect(const std::string& ip) {
        logger_.info("连接中，IP={}", ip);
        // ... 连接逻辑
        logger_.info("连接成功");
    }

    void disconnect() {
        logger_.warn("断开连接");
    }
};

// 数据库模块
class DatabaseModule {
private:
    ILogger logger_;

public:
    DatabaseModule() : logger_("database") {}

    void query(const std::string& sql) {
        logger_.debug("执行查询：{}", sql);
        // ... 查询逻辑
        logger_.info("查询完成，返回 {} 条记录", 100);
    }
};

int main() {
    LoggerManager::getInstance()->set_root_dir("./logs");

    NetworkModule network;
    DatabaseModule database;

    network.connect("192.168.1.1");
    database.query("SELECT * FROM users");

    return 0;
}
```

### 切换日志文件

```cpp
#include "loggermananger/ilogger.h"

int main() {
    LoggerManager::getInstance()->set_root_dir("./logs");

    ILogger logger("network");

    logger.info("写入 network.log");
    logger.flush();

    // 切换日志文件
    logger.set_log_file("database");

    logger.info("写入 database.log");
    logger.flush();

    return 0;
}
```

### 移动语义

```cpp
#include "loggermananger/ilogger.h"
#include <vector>

int main() {
    LoggerManager::getInstance()->set_root_dir("./logs");

    std::vector<ILogger> loggers;
    loggers.reserve(10);

    // 使用移动语义，避免拷贝
    for (int i = 0; i < 10; ++i) {
        ILogger logger("worker_" + std::to_string(i));
        loggers.push_back(std::move(logger));
    }

    loggers[0].info("Worker 0 启动");

    return 0;
}
```

## 与 LoggerManager 的对比

| 特性 | LoggerManager | ILogger |
|------|---------------|---------|
| 调用方式 | `LoggerManager::getInstance()->log("file", Level::INFO, "...")` | `logger.info("...")` |
| 文件名 | 每次调用必须指定 | 构造时绑定，可随时切换 |
| 日志级别 | 每次调用必须指定 | 方法名隐含级别 |
| flush | `flush("file")` 需传文件名 | `flush()` 无参 |
| 适用场景 | 全局日志管理 | 模块化日志 |
| 拷贝 | 禁止 | 禁止 |
| 移动 | 禁止 | 支持 |

## 注意事项

1. **构造前初始化 LoggerManager**：首次使用前需调用 `LoggerManager::getInstance()` 初始化单例
2. **禁止拷贝**：`ILogger` 禁止拷贝，使用移动语义传递
3. **线程安全**：底层调用 `LoggerManager`，已保证线程安全
4. **后缀自动补全**：传入的文件名会自动添加 `.log` 后缀，无需手动添加
