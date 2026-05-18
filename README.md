# QtModules

## 项目概述

通用 C++ 模块库，包含日志管理和设计模式模板。

## 模块

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
- 便捷方法（`debug/info/warn/error`）

**文档**：
- [API 文档](./docs/api/LoggerManager.md)
- [实现文档](./docs/realize/LoggerManager实现文档.md)

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

## 目录结构

```
QtModules/
├── loggermananger/          # 日志管理模块
│   ├── loggermanager.h
│   └── loggermanager.cpp
├── singleton/               # 单例模板
│   └── singleton.h
└── docs/
    ├── api/                 # API 文档
    │   ├── LoggerManager.md
    │   └── Singleton.md
    └── realize/             # 实现文档
        ├── LoggerManager实现文档.md
        └── Singleton实现文档.md
```

## 依赖

- C++17 或更高版本
- spdlog（LoggerManager）
- 标准库：`<memory>`, `<mutex>`, `<iostream>`

## 开发记录

- [2026-05-13] 完成 LoggerManager 和 Singleton 模块开发及文档编写