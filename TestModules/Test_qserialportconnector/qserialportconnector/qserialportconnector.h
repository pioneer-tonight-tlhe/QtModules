#ifndef QSERIALPORTCONNECTOR_H
#define QSERIALPORTCONNECTOR_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QSharedPointer>

#include <QMutex>        // 互斥锁支持
#include <QMutexLocker>  // 互斥锁 RAII 包装

#include "ilogger.h"

/**
 * @brief 串口连接管理器
 *
 * 基于 Qt QSerialPort 的连接管理类，提供自动重连、状态监控、日志记录等功能。
 * 不负责数据读写，只管理连接生命周期。
 */
class QSerialPortConnector : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 连接模式枚举
     */
    enum class ConnectMode {
        AutoReconnect,   // 断开自动重连（默认）
        SingleConnect    // 单次连接，不自动重连
    };

    /**
     * @brief 连接状态枚举
     */
    enum class State {
        Disconnected,    // 已断开
        Connecting,      // 连接中（首次连接或重连）
        Connected,       // 已连接
        Error            // 错误状态（仅用于日志记录，不停留）
    };
    Q_ENUM(State)

    /**
     * @brief 构造函数
     * @param serialPort QSerialPort 共享指针，外部创建并配置参数
     * @param parent 父对象
     * @throws std::invalid_argument 如果 serialPort 为空、端口名未设置或波特率未设置
     */
    explicit QSerialPortConnector(QSharedPointer<QSerialPort> serialPort,
                                  const QString &deviceId,
                                  QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~QSerialPortConnector();

    /**
     * @brief 启动连接管理器
     * @param mode 连接模式，默认 AutoReconnect
     */
    void start(ConnectMode mode = ConnectMode::AutoReconnect);

    /**
     * @brief 停止连接管理器
     */
    void stop();

    /**
     * @brief 断开当前连接（不停止重连机制）
     *
     * AutoReconnect 模式下会自动重连
     */
    void disconnect();

    /**
     * @brief 设置重连时间间隔
     * @param ms 间隔毫秒数
     */
    void setInterval(int ms);

    /**
     * @brief 获取当前状态
     * @return 当前连接状态
     */
    State state() const;

signals:
    /**
     * @brief 状态变化信号
     * @param state 新状态
     */
    void stateChanged(State state);

    /**
     * @brief 错误发生信号
     * @param error 错误描述
     */
    void errorOccurred(const QString &error);

private slots:
    /**
     * @brief 串口错误处理
     * @param error QSerialPort 错误码
     */
    void onSerialError(QSerialPort::SerialPortError error);

    /**
     * @brief 执行重连
     */
    void doReconnect();

private:
    /**
     * @brief 执行单次连接
     */
    void singleConnect();

    /**
     * @brief 设置状态
     * @param newState 新状态
     */
    void setState(State newState);

    /**
     * @brief 启动重连定时器
     */
    void startReconnectTimer();

    /**
     * @brief 停止重连定时器
     */
    void stopReconnectTimer();

    /**
     * @brief 根据错误码获取描述
     * @param error QSerialPort 错误码
     * @return 错误描述字符串
     */
    QString errorString(QSerialPort::SerialPortError error) const;

    QString m_deviceId;                           // 设备ID
    QString m_logPath;                            // 日志路径
    int m_reconnectInterval;                      // 断开重连时间间隔（毫秒）
    QSharedPointer<QSerialPort> m_serialPort;   // 设备引用对象（共享指针管理）
    QTimer* m_reconnectTimer;                     // 重连定时器
    State m_state;                                // 当前状态
    ConnectMode m_connectMode;                    // 连接模式
    ILogger m_logger;                             // 日志记录器

    mutable QMutex m_stateMutex;  // [新增] 保护 m_state 读写的互斥锁
};

#endif // QSERIALPORTCONNECTOR_H
