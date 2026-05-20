# QSerialPortConnector 需求文档

## 1. 概述

`QSerialPortConnector` 是一个基于 Qt 串口通信的连接管理类，提供自动重连、状态监控、错误处理等功能。用于管理 QSerialPort 对象的连接生命周期，确保串口通信的稳定性。

**开发信息**：
- 开发人员：Simon
- 开发时间：2026-05-20
- Qt 版本：5.15.2
- 编译工具：qmake

---

## 2. 功能需求

### 2.1 核心功能

1. **连接管理**
   - 支持两种连接模式：自动重连、单次连接
   - 提供 start/stop 接口控制连接生命周期
   - 支持断开连接（不停止重连机制）
   - 外部只提供 start 方法

2. **自动重连**
   - 可配置重连时间间隔
   - 检测连接状态，断开时自动重连

3. **状态监控**
   - 提供状态枚举：Disconnected、Connecting、Connected、Error
   - 状态变化时发出信号通知
   - 提供查询当前状态的接口

4. **错误处理**
   - 错误发生时发出信号通知
   - 构造函数检测非法输入并抛出异常
   - 根据 `QSerialPort::SerialPortError` 枚举打印对应的错误日志

5. **日志记录**
   - 使用 LoggerManager 打印日志
   - 日志路径可配置
   - 记录连接状态变化、错误信息等

### 2.2 非功能需求

1. **线程安全**：确保多线程环境下的安全性
2. **资源管理**：使用 QSharedPointer 管理 QSerialPort 对象生命周期
3. **异常安全**：构造函数参数验证，失败时抛出异常
4. **易用性**：接口简洁，使用方便

---

## 3. 接口设计

### 3.1 枚举定义

```cpp
enum class ConnectMode {
    AutoReconnect,   // 断开自动重连（默认）
    SingleConnect    // 单次连接
};

enum class State {
    Disconnected,    // 已断开
    Connecting,      // 连接中
    Connected,       // 已连接
    Error            // 错误状态
};
```

### 3.2 公开方法

```cpp
// 构造函数
explicit QSerialPortConnector(QSharedPointer<QSerialPort> serialPort, QObject* parent = nullptr);

// 析构函数
~QSerialPortConnector();

// 启动连接管理器
void start(ConnectMode mode = ConnectMode::AutoReconnect);

// 停止连接管理器
void stop();

// 断开连接（不停止重连机制）
void disconnect();

// 设置重连时间间隔（毫秒）
void setInterval(int ms);

// 获取当前状态
State state() const;
```

### 3.3 私有方法

```cpp
// 执行单次连接
void single_connect();
```

### 3.4 信号

```cpp
// 状态变化信号
void stateChanged(State state);

// 错误发生信号
void errorOccurred(const QString& error);
```

### 3.5 成员变量

```cpp
QString deviceId_;                             // 设备ID
QString logPath_;                              // 日志路径
int reconnectInterval_;                        // 断开重连时间间隔（毫秒）
QSharedPointer<QSerialPort> serialPort_;       // 设备引用对象（共享指针管理）
QTimer* reconnectTimer_;                       // 重连定时器
State state_;                                  // 当前状态
ConnectMode connectMode_;                      // 连接模式
```

---

## 4. 异常处理

### 4.1 构造函数异常

构造函数检测以下情况并抛出 `std::invalid_argument` 异常：

1. **空指针检测**
   - `QSharedPointer<QSerialPort>` 为 null
   - 异常信息：`"QSerialPort pointer is null"`

2. **端口名称未设置**
   - `QSerialPort::portName()` 为空
   - 异常信息：`"QSerialPort port name is not set"`

3. **波特率未设置**
   - `QSerialPort::baudRate()` 为 `UnknownBaud`
   - 异常信息：`"QSerialPort baud rate is not set"`

### 4.2 运行时错误

运行时错误通过信号通知，不抛出异常：

- 连接失败：发出 `errorOccurred` 信号，状态切换为 `Error`

**错误日志打印**：
- 当发生错误时，根据 `QSerialPort::SerialPortError` 枚举值打印对应的错误日志
- 使用 LoggerManager 记录错误信息到指定日志路径
- 连接相关错误类型：
  - `DeviceNotFoundError` - 设备未找到
  - `PermissionError` - 权限错误
  - `OpenError` - 打开设备失败
  - `ResourceError` - 资源错误
  - `TimeoutError` - 超时错误

---

## 5. 使用场景

### 5.1 自动重连模式

```cpp
// 创建并配置 QSerialPort
auto serialPort = QSharedPointer<QSerialPort>::create();
serialPort->setPortName("COM1");
serialPort->setBaudRate(QSerialPort::Baud9600);
serialPort->setDataBits(QSerialPort::Data8);
serialPort->setParity(QSerialPort::NoParity);
serialPort->setStopBits(QSerialPort::OneStop);

// 创建连接器（自动重连模式）
auto connector = new QSerialPortConnector(serialPort);
connector->setInterval(5000);  // 5秒重连间隔

// 连接状态变化处理
QObject::connect(connector, &QSerialPortConnector::stateChanged, [](State state) {
    qDebug() << "状态变化:" << static_cast<int>(state);
});

// 启动连接
connector->start();  // 默认 AutoReconnect 模式
```

### 5.2 单次连接模式

```cpp
auto connector = new QSerialPortConnector(serialPort);

// 单次连接，不自动重连
connector->start(ConnectMode::SingleConnect);

// 断开后需要再次调用 start
connector->start(ConnectMode::SingleConnect);
```

### 5.3 临时断开

```cpp
// 临时断开连接（不停止重连机制）
connector->disconnect();

// 如果是 AutoReconnect 模式，会自动重连
// 如果需要完全停止，调用 stop()
connector->stop();
```

---

## 6. 设计原则

1. **单一职责**：只负责连接管理，不处理数据读写
2. **依赖注入**：通过构造函数注入 QSerialPort，不负责创建
3. **智能指针**：使用 QSharedPointer 管理对象生命周期
4. **信号驱动**：通过信号通知状态变化和错误
5. **异常安全**：构造函数参数验证，失败时立即抛出异常

---

## 7. 依赖

- Qt 5.15.2
- QSerialPort 模块
- C++11 或更高版本（支持智能指针和枚举类）

---

## 8. 后续扩展

1. 添加数据读写接口（如需要）
2. 支持超时配置
3. 添加日志记录功能
4. 支持连接统计信息（连接次数、成功率等）
