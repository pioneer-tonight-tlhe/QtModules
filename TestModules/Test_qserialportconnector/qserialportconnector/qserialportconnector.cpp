#include "qserialportconnector.h"

#include <stdexcept>

/**
 * @brief 构造函数
 *
 * 初始化流程：
 * 1. 保存共享指针
 * 2. 创建重连定时器
 * 3. 参数校验（空指针、端口名、波特率）
 * 4. 连接 QSerialPort 错误信号
 * 5. 记录创建日志
 */
QSerialPortConnector::QSerialPortConnector(QSharedPointer<QSerialPort> serialPort,
                                           const QString &deviceId,
                                           QObject *parent)
    : QObject(parent)
    , m_deviceId(deviceId)
    ,m_logPath("connector/connector_" + deviceId)
    , m_reconnectInterval(5000)
    , m_serialPort(serialPort)
    , m_reconnectTimer(new QTimer(this))
    , m_state(State::Disconnected)
    , m_connectMode(ConnectMode::AutoReconnect)
    , m_logger(m_logPath.toStdString())
{


    // ========== 参数校验 ==========

    // 校验1：空指针检测
    if (!m_serialPort) {
        throw std::invalid_argument("QSerialPort pointer is null");
    }

    // 校验2：端口名未设置
    if (m_serialPort->portName().isEmpty()) {
        throw std::invalid_argument("QSerialPort port name is not set");
    }

    // 校验3：波特率未设置
    if (m_serialPort->baudRate() == QSerialPort::UnknownBaud) {
        throw std::invalid_argument("QSerialPort baud rate is not set");
    }

    // ========== 初始化定时器 ==========

    // 设置为单次触发模式，超时后自动停止
    m_reconnectTimer->setSingleShot(true);

    // 定时器超时触发 doReconnect() 槽
    connect(m_reconnectTimer, &QTimer::timeout, this, &QSerialPortConnector::doReconnect);

    // ========== 连接串口错误信号 ==========

    // 当 QSerialPort 发生错误时errorOccurred， 调用 onSerialError() 处理
    connect(m_serialPort.data(), &QSerialPort::errorOccurred,
            this, &QSerialPortConnector::onSerialError);

    // ========== 记录创建日志 ==========

    m_logger.info("[ID：{}]QSerialPortConnector created, port={}, baud={}",
                  m_deviceId.toStdString(),
                  m_serialPort->portName().toStdString(),
                  m_serialPort->baudRate());
}

/**
 * @brief 析构函数
 *
 * 自动调用 stop() 确保资源释放
 */
QSerialPortConnector::~QSerialPortConnector()
{
    stop();
    m_logger.info("QSerialPortConnector destroyed");
}

/**
 * @brief 启动连接管理器
 * @param mode 连接模式
 *
 * 启动流程：
 * 1. 检查当前状态，防止重复启动
 * 2. 保存连接模式
 * 3. 执行单次连接
 */
void QSerialPortConnector::start(ConnectMode mode)
{
    // 防止重复启动：已在连接中或已连接时直接返回
    QMetaObject::invokeMethod(this,[this,mode](){

    if (m_state == State::Connected || m_state == State::Connecting) {
        m_logger.warn("[ID：{}]Already connected or connecting, ignore start()"
                      ,m_deviceId.toStdString());
        return;
    }

    // 保存连接模式
    m_connectMode = mode;

    // 执行连接
    singleConnect();
    },Qt::QueuedConnection);
}

/**
 * @brief 停止连接管理器
 *
 * 停止流程：
 * 1. 停止重连定时器
 * 2. 关闭串口（如果打开）
 * 3. 状态变为 Disconnected
 * 4. 记录日志
 */
void QSerialPortConnector::stop()
{
    // 停止重连定时器，防止后续触发
    QMetaObject::invokeMethod(this, [this]() {
    stopReconnectTimer();

    // 关闭串口
    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->close();
    }

    // 更新状态
    setState(State::Disconnected);
    m_logger.info("[ID：{}]Connector stopped",m_deviceId.toStdString());
    }, Qt::QueuedConnection);
}

/**
 * @brief 断开当前连接（不停止重连机制）
 *
 * 断开流程：
 * 1. 关闭串口
 * 2. 状态变为 Disconnected
 * 3. AutoReconnect 模式下自动进入重连
 *
 * 与 stop() 的区别：不停止重连定时器，AutoReconnect 模式下会继续重连
 */
void QSerialPortConnector::disconnect()
{
    // 检查串口是否打开
    QMetaObject::invokeMethod(this, [this]() {
    if (!m_serialPort || !m_serialPort->isOpen()) {
        return;
    }

    // 关闭串口
    m_serialPort->close();
    setState(State::Disconnected);
    m_logger.info("[ID：{}]Disconnected by request",m_deviceId.toStdString());

    // AutoReconnect 模式下：自动重连
    if (m_connectMode == ConnectMode::AutoReconnect) {
        setState(State::Connecting);
        startReconnectTimer();
    }
    }, Qt::QueuedConnection);
    // SingleConnect 模式下：不做任何操作，保持 Disconnected
}

/**
 * @brief 设置重连时间间隔
 * @param ms 间隔毫秒数
 */
void QSerialPortConnector::setInterval(int ms)
{
    QMetaObject::invokeMethod(this, [this, ms]() {
        m_reconnectInterval = ms;
        m_logger.info("[ID：{}]Reconnect interval set to {} ms", ms,m_deviceId.toStdString());
    }, Qt::QueuedConnection);
}

/**
 * @brief 获取当前状态
 * @return 当前连接状态
 */
QSerialPortConnector::State QSerialPortConnector::state() const
{
    QMutexLocker locker(&m_stateMutex);
    return m_state;
}

/**
 * @brief 串口错误处理槽
 * @param error QSerialPort 错误码
 *
 * 处理流程：
 * 1. 忽略 NoError
 * 2. 获取错误描述，记录日志
 * 3. 状态变为 Error（瞬态，用于日志）
 * 4. 关闭串口
 * 5. AutoReconnect 模式下：状态变为 Connecting，启动重连定时器
 * 6. SingleConnect 模式下：状态变为 Disconnected
 */
void QSerialPortConnector::onSerialError(QSerialPort::SerialPortError error)
{
    // 忽略无错误的情况
    if (error == QSerialPort::NoError) {
        return;
    }

    // 获取错误描述并记录日志
    QString reason = errorString(error);
    m_logger.error("[ID：{}]Serial error: {}",m_deviceId.toStdString(), reason.toStdString());

    // Error 状态仅用于日志记录，瞬态，不停留
    setState(State::Error);
    emit errorOccurred(reason);

    // 关闭串口，释放资源
    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->close();
    }

    // 根据连接模式决定下一步
    if (m_connectMode == ConnectMode::AutoReconnect) {
        // 自动重连：进入 Connecting 状态，启动定时器
        setState(State::Connecting);
        startReconnectTimer();
    } else {
        // 单次连接：保持 Disconnected
        setState(State::Disconnected);
    }
}

/**
 * @brief 执行重连槽
 *
 * 重连定时器触发时调用
 *
 * 流程：
 * 1. 安全检查：只有 Connecting 状态才执行
 * 2. 检查指针有效性
 * 3. 执行单次连接
 */
void QSerialPortConnector::doReconnect()
{
    // 安全检查：只有 Connecting 状态才执行
    // 防止定时器延迟触发时状态已改变
    if (m_state != State::Connecting) {
        return;
    }

    // 检查指针有效性
    if (!m_serialPort) {
        m_logger.error("[ID：{}]Serial port is null, cannot reconnect",m_deviceId.toStdString());
        return;
    }

    // 执行连接
    singleConnect();
}

/**
 * @brief 执行单次连接
 *
 * 同步调用 QSerialPort::open()，立即返回结果
 *
 * 成功流程：
 *   状态 Connecting → Connected，记录日志
 *
 * 失败流程（AutoReconnect）：
 *   状态 Connecting → Error（瞬态，emit errorOccurred）→ Connecting，启动定时器
 *
 * 失败流程（SingleConnect）：
 *   状态 Connecting → Error（瞬态，emit errorOccurred）→ Disconnected
 */
void QSerialPortConnector::singleConnect()
{
    // 设置状态为 Connecting
    setState(State::Connecting);
    m_logger.info("[ID：{}]Connecting to {}...",
                  m_deviceId.toStdString(),
                  m_serialPort->portName().toStdString());

    // 同步打开串口
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        // ========== 连接成功 ==========

        setState(State::Connected);
        m_logger.info("[ID：{}]Connected successfully",m_deviceId.toStdString());
    } else {
        // ========== 连接失败 ==========

        // 获取错误原因
        QString reason = m_serialPort->errorString();
        m_logger.error("[ID：{}]Connection failed: {}",m_deviceId.toStdString(), reason.toStdString());

        // Error 状态仅用于日志，瞬态
        setState(State::Error);
        emit errorOccurred(QString("Connection failed: %1").arg(reason));

        // 根据模式决定下一步
        if (m_connectMode == ConnectMode::AutoReconnect) {
            // 自动重连：继续 Connecting，启动定时器等待下次重试
            setState(State::Connecting);
            startReconnectTimer();
        } else {
            // 单次连接：保持 Disconnected
            setState(State::Disconnected);
        }
    }
}

/**
 * @brief 设置状态（内部统一更新）
 * @param newState 新状态
 *
 * 功能：
 * 1. 防止重复设置相同状态
 * 2. 状态变化时 emit stateChanged() 信号
 * 3. 记录状态变化日志
 */
void QSerialPortConnector::setState(State newState)
{
    {
        QMutexLocker locker(&m_stateMutex);  // [新增] 加锁
        if (m_state == newState) {
            return;
        }
        m_state = newState;
    }
        emit stateChanged(m_state);

        // 记录状态变化日志
        const char* stateNames[] = {"Disconnected", "Connecting", "Connected", "Error"};
        m_logger.debug("[ID：{}]State changed to {}",
                       m_deviceId.toStdString(),
                       stateNames[static_cast<int>(m_state)]);

}

/**
 * @brief 启动重连定时器
 *
 * 使用 m_reconnectInterval 作为间隔，单次触发
 * 超时后自动调用 doReconnect()
 */
void QSerialPortConnector::startReconnectTimer()
{
    if (m_reconnectTimer) {
        m_logger.debug("[ID：{}]Start reconnect timer, interval={}ms",
                       m_deviceId.toStdString(),
                       m_reconnectInterval);
        m_reconnectTimer->start(m_reconnectInterval);
    }
}

/**
 * @brief 停止重连定时器
 *
 * 如果定时器正在运行，立即停止
 */
void QSerialPortConnector::stopReconnectTimer()
{
    if (m_reconnectTimer && m_reconnectTimer->isActive()) {
        m_reconnectTimer->stop();
        m_logger.debug("[ID：{}]Reconnect timer stopped",m_deviceId.toStdString());
    }
}

/**
 * @brief 根据错误码获取描述
 * @param error QSerialPort 错误码
 * @return 错误描述字符串
 *
 * 将 QSerialPort::SerialPortError 枚举转换为可读字符串
 */
QString QSerialPortConnector::errorString(QSerialPort::SerialPortError error) const
{
    switch (error) {
    case QSerialPort::DeviceNotFoundError:
        return QStringLiteral("Device not found");
    case QSerialPort::PermissionError:
        return QStringLiteral("Permission denied");
    case QSerialPort::OpenError:
        return QStringLiteral("Failed to open device");
    case QSerialPort::ParityError:
        return QStringLiteral("Parity error");
    case QSerialPort::FramingError:
        return QStringLiteral("Framing error");
    case QSerialPort::BreakConditionError:
        return QStringLiteral("Break condition");
    case QSerialPort::WriteError:
        return QStringLiteral("Write error");
    case QSerialPort::ReadError:
        return QStringLiteral("Read error");
    case QSerialPort::ResourceError:
        return QStringLiteral("Resource error");
    case QSerialPort::UnsupportedOperationError:
        return QStringLiteral("Unsupported operation");
    case QSerialPort::UnknownError:
        return QStringLiteral("Unknown error");
    case QSerialPort::TimeoutError:
        return QStringLiteral("Timeout");
    case QSerialPort::NotOpenError:
        return QStringLiteral("Device not open");
    default:
        return QStringLiteral("Unknown error");
    }
}
