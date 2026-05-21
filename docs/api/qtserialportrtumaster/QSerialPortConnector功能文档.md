# QSerialPortConnector API 文档

**开发信息**：
- 开发人员：李卓
- 开发时间：2026-05-21

## 概述

`QSerialPortConnector` 是基于 Qt `QSerialPort` 的串口连接管理类，通过封装串口生命周期、自动重连机制和状态机管理，提供稳定可靠的串口连接服务。适用于工业上位机、物联网网关等需要长期保持串口通信的场景。

**设计目的**：
- 简化串口连接管理：无需手动处理打开、关闭、错误、重连逻辑
- 状态透明化：通过信号实时通知连接状态变化
- 故障自愈：自动重连机制应对临时性串口故障
- 模块化日志：每个连接器实例写入独立日志文件，便于多设备排查

## 主要特性

- 基于 `QSharedPointer<QSerialPort>` 共享指针，支持外部灵活管理串口对象
- 两种连接模式：自动重连（默认）和单次连接
- 四种连接状态：已断开、连接中、已连接、错误（瞬态）
- 线程安全：公共接口通过 `QMetaObject::invokeMethod` 排队到对象线程
- 状态读写受 `QMutex` 保护
- 集成 `ILogger` 模块化日志，自动按设备ID分类
- 构造函数参数校验，无效配置直接抛异常

## 连接模式

| 模式 | 枚举值 | 说明 |
|------|--------|------|
| 自动重连 | `ConnectMode::AutoReconnect` | 断开或连接失败后自动重试（默认） |
| 单次连接 | `ConnectMode::SingleConnect` | 仅连接一次，失败或断开后不再重试 |

## 连接状态

| 状态 | 枚举值 | 说明 |
|------|--------|------|
| 已断开 | `State::Disconnected` | 串口未打开或已关闭 |
| 连接中 | `State::Connecting` | 正在尝试打开串口或重连中 |
| 已连接 | `State::Connected` | 串口成功打开，可读写数据 |
| 错误 | `State::Error` | 瞬态状态，仅用于日志记录，不停留 |

## 公共 API

### 构造函数

```cpp
explicit QSerialPortConnector(QSharedPointer<QSerialPort> serialPort,
                              const QString &deviceId,
                              QObject *parent = nullptr);
```

**说明**：构造串口连接管理器实例，绑定串口对象与设备标识。

**参数**：
- `serialPort`：`QSerialPort` 共享指针，需预先配置端口名和波特率
- `deviceId`：设备唯一标识，用于日志分类和状态区分
- `parent`：父对象，纳入 Qt 对象树管理生命周期

**异常**：
- `std::invalid_argument`：serialPort 为空指针
- `std::invalid_argument`：端口名未设置（`portName().isEmpty()`）
- `std::invalid_argument`：波特率未设置（`baudRate() == UnknownBaud`）

**示例**：
```cpp
QSharedPointer<QSerialPort> serial(new QSerialPort());
serial->setPortName("COM3");
serial->setBaudRate(QSerialPort::Baud9600);

QSerialPortConnector* connector = new QSerialPortConnector(serial, "PLC_01", this);
```

---

### 析构函数

```cpp
~QSerialPortConnector();
```

**说明**：自动调用 `stop()` 确保资源释放，记录销毁日志。

---

### 启动连接

```cpp
void start(ConnectMode mode = ConnectMode::AutoReconnect);
```

**说明**：启动连接管理器，根据模式执行首次连接或进入重连循环。

**参数**：
- `mode`：连接模式，默认 `AutoReconnect`

**行为**：
- 当前为 `Connected` 或 `Connecting` 时，记录警告并忽略
- 保存连接模式后执行 `singleConnect()`

**线程安全**：通过 `Qt::QueuedConnection` 排队到对象线程执行

**示例**：
```cpp
// 自动重连模式（默认）
connector->start();

// 单次连接模式
connector->start(QSerialPortConnector::ConnectMode::SingleConnect);
```

---

### 停止连接

```cpp
void stop();
```

**说明**：彻底停止连接管理器，关闭串口，停止重连定时器，状态变为 `Disconnected`。

**行为**：
- 停止重连定时器
- 关闭串口（如果打开）
- 设置状态为 `Disconnected`
- 记录停止日志

**与 `disconnect()` 区别**：`stop()` 会停止重连机制，不再自动重连

**线程安全**：通过 `Qt::QueuedConnection` 排队到对象线程执行

**示例**：
```cpp
connector->stop();  // 彻底停止，不再重连
```

---

### 断开连接

```cpp
void disconnect();
```

**说明**：主动断开当前连接，但保留重连机制（AutoReconnect 模式下）。

**行为**：
- 关闭串口
- 设置状态为 `Disconnected`
- `AutoReconnect` 模式下：自动进入 `Connecting` 并启动定时器
- `SingleConnect` 模式下：保持 `Disconnected`

**与 `stop()` 区别**：`disconnect()` 不停止重连定时器，AutoReconnect 模式下会继续重连

**线程安全**：通过 `Qt::QueuedConnection` 排队到对象线程执行

**示例**：
```cpp
connector->disconnect();  // 断开，但 AutoReconnect 模式下会继续重连
```

---

### 设置重连间隔

```cpp
void setInterval(int ms);
```

**说明**：设置自动重连的时间间隔。

**参数**：
- `ms`：间隔毫秒数，默认 5000ms

**线程安全**：通过 `Qt::QueuedConnection` 排队到对象线程执行

**示例**：
```cpp
connector->setInterval(3000);  // 每 3 秒重试一次
```

---

### 获取当前状态

```cpp
State state() const;
```

**说明**：获取当前连接状态。

**返回值**：当前 `State` 枚举值

**线程安全**：使用 `QMutexLocker` 保护读操作

**示例**：
```cpp
auto s = connector->state();
if (s == QSerialPortConnector::State::Connected) {
    // 可以发送数据
}
```

---

## 信号

### 状态变化信号

```cpp
void stateChanged(State state);
```

**说明**：连接状态发生变化时发射。

**参数**：
- `state`：新状态

**触发时机**：
- `Disconnected` → `Connecting`（启动连接）
- `Connecting` → `Connected`（连接成功）
- `Connecting` → `Error` → `Connecting`（连接失败，自动重连）
- `Connecting` → `Error` → `Disconnected`（连接失败，单次模式）
- `Connected` → `Error` → `Connecting`（串口错误，自动重连）
- `Connected` → `Disconnected`（主动断开或停止）

**示例**：
```cpp
connect(connector, &QSerialPortConnector::stateChanged, this, [](auto state) {
    const char* names[] = {"Disconnected", "Connecting", "Connected", "Error"};
    qDebug() << "State changed to:" << names[static_cast<int>(state)];
});
```

---

### 错误发生信号

```cpp
void errorOccurred(const QString &error);
```

**说明**：串口发生错误时发射，携带错误描述。

**参数**：
- `error`：错误描述字符串（英文）

**触发时机**：
- `QSerialPort::errorOccurred` 信号触发（非 `NoError`）
- `singleConnect()` 中 `open()` 失败

**示例**：
```cpp
connect(connector, &QSerialPortConnector::errorOccurred, this, [](const QString& err) {
    qDebug() << "Serial error:" << err;
    // 可在此通知 UI 显示错误提示
});
```

---

## 使用示例

### 基础使用

```cpp
#include "qserialportconnector.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 初始化 LoggerManager（只需一次）
    LoggerManager::getInstance()->set_root_dir("./logs");

    // 创建并配置串口
    QSharedPointer<QSerialPort> serial(new QSerialPort());
    serial->setPortName("COM3");
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);

    // 创建连接管理器
    QSerialPortConnector connector(serial, "PLC_01");

    // 监听状态
    QObject::connect(&connector, &QSerialPortConnector::stateChanged, 
        [](auto state) {
            qDebug() << "State:" << static_cast<int>(state);
        });

    // 监听错误
    QObject::connect(&connector, &QSerialPortConnector::errorOccurred,
        [](const QString& err) {
            qDebug() << "Error:" << err;
        });

    // 启动自动重连
    connector.start();

    return app.exec();
}
```

### 动态调整重连间隔

```cpp
// 初始快速重连
connector->setInterval(1000);   // 1秒
connector->start();

// 连接成功后降低频率（在 stateChanged 槽中）
connect(connector, &QSerialPortConnector::stateChanged, this, [connector](auto state) {
    if (state == QSerialPortConnector::State::Connected) {
        connector->setInterval(30000);  // 连接成功后改为30秒（用于心跳检测）
    }
});
```

---

## 与直接使用 QSerialPort 的对比

| 特性 | 直接使用 QSerialPort | QSerialPortConnector |
|------|---------------------|---------------------|
| 连接管理 | 手动调用 open/close | 封装 start/stop/disconnect |
| 错误处理 | 手动连接 errorOccurred | 自动监听并处理 |
| 自动重连 | 需自行实现定时器逻辑 | 内置 AutoReconnect |
| 状态查询 | 仅通过 isOpen() 判断 | 完整状态机：Disconnected/Connecting/Connected/Error |
| 状态通知 | 无 | stateChanged 信号 |
| 线程安全 | 需自行保证 | invokeMethod 排队 + Mutex 保护 |
| 日志记录 | 无 | 模块化 ILogger，按设备分类 |
| 参数校验 | 运行时可能崩溃 | 构造函数抛异常，提前暴露问题 |

## 注意事项

1. **串口参数外部配置**：`QSerialPort` 的端口名、波特率、数据位、校验位、停止位需在传入前配置完成，本类仅校验端口名和波特率
2. **共享指针生命周期**：确保 `QSerialPort` 共享指针在 `QSerialPortConnector` 生命周期内有效
3. **对象线程归属**：建议将 `QSerialPortConnector` 与 `QSerialPort` 置于同一线程，避免跨线程信号问题
4. **日志初始化**：使用前需初始化 `LoggerManager` 单例，设置根目录
5. **状态信号处理**：`Error` 状态为瞬态，信号接收端不应长期依赖此状态做 UI 展示
6. **析构自动清理**：析构时会调用 `stop()`，但建议显式调用 `stop()` 后再删除，确保时序可控
7. **重连间隔下限**：建议不要设置低于 1000ms 的重连间隔，避免频繁重试占用 CPU

---

## 依赖

- Qt 5.12+ 或 Qt 6.x（`QSerialPort`、`QTimer`、`QSharedPointer`、`QMetaObject`、`QMutex`）
- C++11 或更高版本（lambda、异常）
- `ILogger` 及其依赖（`LoggerManager`、`spdlog`）
- 标准库：`<stdexcept>`、`<QString>`
