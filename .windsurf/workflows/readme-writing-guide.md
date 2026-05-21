---
description: README.md 编写规范指南
---

# README.md 编写规范指南

## 文档结构

QtModules 项目的 README.md 遵循以下标准结构：

```
# 项目名称

## 项目概述

## 模块

### 模块名 - 状态

模块描述

**功能说明**：
- 功能点1
- 功能点2

**适用场景**：
- 场景1
- 场景2

**特性**：[可选]
- 技术特性1
- 技术特性2

**文档**：
- [文档类型](./文档路径)

**开发结果**：[可选，仅开发阶段]
- 结果1
- 结果2

**关联代码**：[可选]
- [代码路径](./代码路径)

**测试项目**：[可选，仅开发阶段]
- [测试项目路径](./测试项目路径)

**开发信息**：
- 开发人员：姓名
- 开发时间：YYYY-MM-DD
- 当前状态：状态描述

## 目录结构

## 依赖

## 测试模块
```

## 各部分编写规范

### 1. 项目标题

- 使用一级标题 `#`
- 格式：`# 项目名称`
- 示例：`# QtModules`

### 2. 项目概述

- 使用二级标题 `## 项目概述`
- 简要描述项目性质、技术栈、编译工具等
- 示例：`Qt 项目相关的 C++ 模块库（Qt 版本：5.15.2，编译工具：qmake）。`

### 3. 模块部分

#### 3.1 模块标题

- 使用三级标题 `###`
- 格式：`### 模块名 - 状态`
- 状态类型：
  - `需求文档阶段` - 仅完成需求文档
  - `开发阶段` - 正在开发中
  - `开发完成` - 开发已完成
  - 无状态后缀 - 已完成的稳定模块

#### 3.2 模块描述

- 模块标题下方的一行简短描述
- 说明模块的主要功能和用途
- 示例：`基于 Qt 串口通信的连接管理类。`

#### 3.3 功能说明

- 使用 `**功能说明**：` 标题
- 使用无序列表列出主要功能
- 每个功能点简洁明了
- 示例：
  ```
  **功能说明**：
  - 支持两种连接模式：自动重连、单次连接
  - 提供 start/stop/disconnect 接口控制连接生命周期
  ```

#### 3.4 适用场景

- 使用 `**适用场景**：` 标题
- 使用无序列表列出适用场景
- 说明模块适合的应用场景
- 示例：
  ```
  **适用场景**：
  - 需要保持稳定串口连接的设备
  - 需要自动重连机制的串口通信
  ```

#### 3.5 特性（可选）

- 使用 `**特性**：` 标题
- 使用无序列表列出技术特性
- 仅当模块有特殊技术特性时添加
- 示例：
  ```
  **特性**：
  - 继承通用 `Singleton<T>` 模板（CRTP 模式）
  - 异步日志写入（spdlog 线程池）
  ```

#### 3.6 文档

- 使用 `**文档**：` 标题
- 使用无序列表列出相关文档
- 使用 Markdown 链接格式 `[文档名](./相对路径)`
- 文档类型：
  - 需求文档
  - 实现文档
  - API 文档
- 示例：
  ```
  **文档**：
  - [需求文档](./docs/requirements/QSerialPortConnector需求文档.md)
  - [实现文档](./docs/realize/qtserialportrtumaster/QSerialPortConnector.md)
  - [API 文档](./docs/api/qtserialportrtumaster/QSerialPortConnector功能文档.md)
  ```

#### 3.7 开发结果（可选）

- 使用 `**开发结果**：` 标题
- 仅在开发阶段或开发完成时添加
- 使用无序列表列出已完成的开发成果
- 示例：
  ```
  **开发结果**：
  - 完成 QSerialPortConnector 类的实现
  - 实现自动重连和单次连接两种模式
  - 实现状态机管理
  - 完成测试项目开发
  ```

#### 3.8 关联代码（可选）

- 使用 `**关联代码**：` 标题
- 使用无序列表列出关联的代码目录
- 使用 Markdown 链接格式
- 示例：
  ```
  **关联代码**：
  - [qtserialportrtumaster/qserialportconnector](./qtserialportrtumaster/qserialportconnector)
  ```

#### 3.9 测试项目（可选）

- 使用 `**测试项目**：` 标题
- 仅在有对应测试项目时添加
- 使用无序列表列出测试项目路径
- 使用 Markdown 链接格式
- 示例：
  ```
  **测试项目**：
  - [TestModules/Test_qserialportconnector](./TestModules/Test_qserialportconnector)
  ```

#### 3.10 开发信息

- 使用 `**开发信息**：` 标题
- 使用无序列表列出开发相关信息
- 必须包含：
  - 开发人员
  - 开发时间（格式：YYYY-MM-DD）
  - 当前状态
- 示例：
  ```
  **开发信息**：
  - 开发人员：李卓
  - 开发时间：2026-05-21
  - 当前状态：开发完成
  ```

### 4. 目录结构

- 使用二级标题 `## 目录结构`
- 使用代码块展示目录树
- 使用 `#` 添加注释说明
- 示例：
  ```
  ## 目录结构

  ```
  QtModules/
  ├── loggermananger/          # 日志管理模块
  │   ├── loggermanager.h
  │   ├── loggermanager.cpp
  │   └── ilogger.h           # 日志接口类
  └── docs/
      ├── api/                 # API 文档
      └── realize/             # 实现文档
  ```
  ```

### 5. 依赖

- 使用二级标题 `## 依赖`
- 使用无序列表列出项目依赖
- 包含版本信息
- 示例：
  ```
  ## 依赖

  - C++17 或更高版本
  - spdlog（LoggerManager）
  - Qt 5.15.2（QSerialPortConnector）
  - 标准库：`<memory>`, `<mutex>`, `<iostream>`
  ```

### 6. 测试模块

- 使用二级标题 `## 测试模块`
- 可以使用三级标题 `###` 分组不同的测试项目
- 包含测试功能和目录结构
- 示例：
  ```
  ## 测试模块

  ### Test_LoggerManager

  测试开发模块的代码目录，用于验证各模块功能的正确性和稳定性。

  **目录结构**：
  ```
  TestModules/
  └── Test_LoggerManager/
      ├── loggermananger/
      ├── main.cpp
      └── Test_LoggerManager.pro
  ```

  **说明**：
  - Test_LoggerManager 是一个 Qt 应用程序
  - 包含 LoggerManager 和 Singleton 的源码副本
  ```

## 命名规范

### 文档命名

- API 文档：`模块名功能文档.md`
- 实现文档：`模块名实现文档.md`
- 需求文档：`模块名需求文档.md`

### 目录结构

- 源码：小写字母，使用下划线分隔（如 `loggermananger`）
- 文档：小写字母，使用下划线分隔（如 `qtserialportrtumaster`）
- 测试项目：`Test_模块名`（如 `Test_LoggerManager`）

### 状态标识

- 需求文档阶段：`- 需求文档阶段`
- 开发阶段：`- 开发阶段`
- 开发完成：`- 开发完成`
- 稳定模块：无后缀

## 注意事项

1. **同一模块多阶段**：如果一个模块经历了多个开发阶段（如需求阶段、开发阶段），可以创建多个条目，每个条目对应一个阶段
2. **文档路径**：所有文档路径使用相对路径，以 `./` 开头
3. **开发人员**：支持多个开发人员，每个阶段可以有不同的开发人员
4. **时间格式**：统一使用 `YYYY-MM-DD` 格式
5. **代码格式**：代码类名、方法名使用反引号包裹，如 `LoggerManager::getInstance()`
6. **链接格式**：使用 Markdown 链接格式 `[显示文本](路径)`
7. **列表缩进**：保持一致的缩进层级
8. **空行规范**：各个部分之间保持适当的空行，提高可读性

## 示例模板

### 完整模块示例（开发完成）

```
### QSerialPortConnector - 开发完成

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
```

### 简化模块示例（稳定模块）

```
### LoggerManager

高性能线程安全日志管理器，适用于需要稳定日志记录的服务端应用。

**功能说明**：
- 提供统一的日志写入接口，支持多级别日志（TRACE/DEBUG/INFO/WARN/ERROR/CRITICAL）
- 异步写入机制，不阻塞业务线程，支持高并发场景
- 按日期自动分目录存储，便于归档和检索
- 日志文件达到指定大小时自动回滚，保留历史文件
- 后台线程自动清理过期日志，避免磁盘占用过多

**特性**：
- 继承通用 `Singleton<T>` 模板（CRTP 模式）
- 异步日志写入（spdlog 线程池）
- 按日期分目录存储
- 日志文件大小回滚
- 自动清理过期日志

**文档**：
- [API 文档](./docs/api/LoggerManager功能文档.md)
- [实现文档](./docs/realize/LoggerManager实现文档.md)

**开发信息**：
- 开发人员：Simon
- 开发时间：2026-05-19
```

## 维护指南

1. **新增模块**：按照完整模块示例添加新模块
2. **模块状态更新**：及时更新模块标题中的状态标识
3. **文档更新**：当有新文档时，及时更新文档链接
4. **开发进度**：开发过程中及时更新开发结果
5. **测试项目**：新增测试项目时添加测试项目链接
6. **目录结构**：新增目录或文件时更新目录结构部分
