#ifdef _WIN32
// 排除 RPC 定义，避免与 std::byte 冲突
#define NO_RPC
// 减少 windows.h 的内容，避免更多冲突
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "loggermanager.h"

#include <ILogger.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ILogger log("hello log");
    log.info("hello word!");
    log.warn("warn message.");
    log.error("error message.");
    log.critical("critical message.");
}

MainWindow::~MainWindow()
{
    delete ui;
}
