#ifdef _WIN32
#define NO_RPC
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ========== QSerialPortConnector 测试 ==========
    try {
        auto serialPort = QSharedPointer<QSerialPort>::create();
        serialPort->setPortName("COM9");
        serialPort->setBaudRate(QSerialPort::Baud9600);

        m_connector = new QSerialPortConnector(serialPort, "1", this);
        m_connector->setInterval(10000);

        // 连接状态变化信号，观察日志
        connect(m_connector, &QSerialPortConnector::stateChanged,
                [](QSerialPortConnector::State state) {
                    qDebug() << "state changed:" << static_cast<int>(state);
                });

        // 连接错误信号
        connect(m_connector, &QSerialPortConnector::errorOccurred,
                [](const QString &err) {
                    qDebug() << "error:" << err;
                });

        // 单次连接，不自动重连，方便观察日志
        m_connector->start(QSerialPortConnector::ConnectMode::AutoReconnect);

    } catch (const std::invalid_argument &e) {
        qDebug() << "构造失败:" << e.what();
    }
    // ========== 测试结束 ==========
}

MainWindow::~MainWindow()
{
    delete ui;
}
