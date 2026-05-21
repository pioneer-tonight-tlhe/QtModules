# QtModules

## 项目概述

Qt 项目相关的 C++ 模块库（Qt 版本：5.15.2，编译工具：qmake）。

## 模块

### QSerialPortConnector - 开发阶段

基于 Qt 串口通信的连接管理类。

**功能说明**：
- 支持两种连接模式：自动重连、单次连接
- 提供 start/stop/disconnect 接口控制连接生命周期
- 可配置重连时间间隔，自动检测连接状态并重连
- 提供状态枚举（Disconnected、Connecting、Connected、Error）
- 状态变化时发出信号通知
- 使用 LoggerManager 记录连接状态变化和错误信息
- 根据 QSerialPort::SerialPortError 枚举打印对应的错误日志
- 使用 QSharedPointer 管理 QSerialPort 对象生命周期

**适用场景**：
- 需要保持稳定串口连接的设备
- 需要自动重连机制的串口通信
- 需要监控连接状态的设备管理

**文档**：
- [实现文档](./docs/realize/qtserialportrtumaster/QSerialPortConnector.md)
- [API 文档](./docs/api/qtserialportrtumaster/QSerialPortConnector功能文档.md)

**开发结果**：
- 完成 QSerialPortConnector 类的实现
- 实现自动重连和单次连接两种模式
- 实现状态机管理（Disconnected、Connecting、Connected、Error）
- 实现线程安全的公共接口
- 集成 ILogger 模块化日志
- 完成测试项目开发

**关联代码**：
- [qtserialportrtumaster/qserialportconnector](./qtserialportrtumaster/qserialportconnector)

**测试项目**：
- [TestModules/Test_qserialportconnector](./TestModules/Test_qserialportconnector)

**开发信息**：
- 开发人员：李卓
- 开发时间：2026-05-21
- 当前状态：开发完成

### QSerialPortConnector - 需求文档阶段

基于 Qt 串口通信的连接管理类。

**功能说明**：
- 支持两种连接模式：自动重连、单次连接
- 提供 start/stop/disconnect 接口控制连接生命周期
- 可配置重连时间间隔，自动检测连接状态并重连
- 提供状态枚举（Disconnected、Connecting、Connected、Error）
- 状态变化时发出信号通知
- 使用 LoggerManager 记录连接状态变化和错误信息
- 根据 QSerialPort::SerialPortError 枚举打印对应的错误日志
- 使用 QSharedPointer 管理 QSerialPort 对象生命周期

**适用场景**：
- 需要保持稳定串口连接的设备
- 需要自动重连机制的串口通信
- 需要监控连接状态的设备管理

**文档**：
- [需求文档](./docs/requirements/QSerialPortConnector需求文档.md)
- [实现文档](./docs/realize/qtserialportrtumaster/QSerialPortConnector.md)
- [API 文档](./docs/api/qtserialportrtumaster/QSerialPortConnector功能文档.md)

**关联代码**：
- [qtserialportrtumaster/qserialportconnector](./qtserialportrtumaster/qserialportconnector)

**开发信息**：
- 开发人员：Simon
- 开发时间：2026-05-20
- 当前状态：需求文档阶段

### LoggerManager

高性能线程安全日志管理器，适用于需要稳定日志记录的服务端应用。

**功能说明**：
- 提供统一的日志写入接口，支持多级别日志（TRACE/DEBUG/INFO/WARN/ERROR/CRITICAL）
- 异步写入机制，不阻塞业务线程，支持高并发场景
- 按日期自动分目录存储，便于归档和检索
- 日志文件达到指定大小时自动回滚，保留历史文件
- 后台线程自动清理过期日志，避免磁盘占用过多
- 支持日志汇总到 trace 文件，便于全局追踪
- 支持 WARN 和 ERROR 级别日志分离到独立文件，便于问题排查

**特性**：
- 继承通用 `Singleton<T>` 模板（CRTP 模式）
- 异步日志写入（spdlog 线程池）
- 按日期分目录存储
- 日志文件大小回滚
- 自动清理过期日志

**文档**：
- [API 文档](./docs/api/LoggerManager.md)
- [实现文档](./docs/realize/LoggerManager实现文档.md)

**开发信息**：
- 开发人员：Simon
- 开发时间：2026-05-19

### ILogger

对 `LoggerManager` 的轻量封装，提供无需重复指定路径和级别的简化写入接口。

**功能说明**：
- 构造时绑定日志文件名，自动补全 `.log` 后缀
- 提供 6 个级别的写入方法（trace、debug、info、warn、error、critical），无需传文件名和级别
- 支持格式化字符串，使用 `{}` 占位符
- 提供 `set_log_file()` 切换日志文件
- 提供 `flush()` 无参刷新方法
- 禁止拷贝，支持移动语义

**适用场景**：
- 每个模块持有一个 ILogger 实例，写入独立日志文件
- 需要简化日志调用代码的场景
- 需要语义化日志接口的场景

**文档**：
- [API 文档](./docs/api/ILogger.md)
- [实现文档](./docs/realize/ILogger实现文档.md)

**开发信息**：
- 开发人员：Simon
- 开发时间：2026-05-19

### Singleton

通用线程安全单例模板，用于需要确保全局唯一实例的场景。

**功能说明**：
- 通过模板和 CRTP 模式，让任意类轻松变为单例
- 使用 `std::call_once` 保证多线程环境下只创建一次实例
- 使用 `std::shared_ptr` 自动管理实例生命周期，防止内存泄漏
- 禁止拷贝和赋值，防止意外复制导致多实例问题
- 提供地址打印方法，便于调试验证单例唯一性

**适用场景**：
- 配置管理类
- 数据库连接池
- 日志管理器
- 全局资源管理器

**特性**：
- CRTP 模式（奇异递归模板模式）
- 基于 `std::call_once` 线程安全
- `std::shared_ptr` 管理生命周期
- 禁止拷贝和赋值

**文档**：
- [API 文档](./docs/api/Singleton.md)
- [实现文档](./docs/realize/Singleton实现文档.md)

**开发信息**：
- 开发人员：Simon
- 开发时间：2026-05-19

## 目录结构

```
QtModules/
├── loggermananger/          # 日志管理模块
│   ├── loggermanager.h
│   ├── loggermanager.cpp
│   └── ilogger.h           # 日志接口类
├── singleton/               # 单例模板
│   └── singleton.h
├── TestModules/             # 测试模块
│   └── Test_LoggerManager/  # LoggerManager 测试项目
└── docs/
    ├── api/                 # API 文档
    │   ├── LoggerManager.md
    │   ├── ILogger.md
    │   └── Singleton.md
    ├── realize/             # 实现文档
    │   ├── LoggerManager实现文档.md
    │   ├── ILogger实现文档.md
    │   └── Singleton实现文档.md
    └── requirements/        # 需求文档
        └── QSerialPortConnector需求文档.md
```

## 依赖

- C++17 或更高版本
- spdlog（LoggerManager）
- 标准库：`<memory>`, `<mutex>`, `<iostream>`

## 测试模块

### TestModules

测试开发模块的代码目录，用于验证各模块功能的正确性和稳定性。

**目录结构**：
```
TestModules/
└── Test_LoggerManager/       # LoggerManager 测试项目
    ├── loggermananger/       # LoggerManager 源码副本
    ├── singleton.h          # Singleton 模板副本
    ├── main.cpp              # 测试入口
    ├── mainwindow.*          # Qt 界面
    └── Test_LoggerManager.pro # Qt 项目文件
```

**说明**：
- Test_LoggerManager 是一个 Qt 应用程序，提供图形界面测试 LoggerManager 的各项功能
- 包含 LoggerManager 和 Singleton 的源码副本，用于独立测试
- 支持配置日志系统、启用自动 trace、错误分离等功能的测试验证