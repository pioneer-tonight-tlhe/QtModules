#include "mainwindow.h"

#include <QApplication>
#include <loggermanager.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    // 获取单例
    auto logger = LoggerManager::getInstance();

    // 配置日志系统
    logger->set_root_dir("./logs");              // 设置根目录
    logger->set_rotation(20 * 1024 * 1024, 10);  // 单文件 20MB，保留 10 个
    logger->set_retention_days(30);              // 保留 30 天
    logger->set_trace_filename("trace");       // trace 文件名

    // 启用自动 trace 和错误分离
    logger->enable_auto_trace(true);
    logger->enable_warn_error_split(true);

    // 直接使用管理类写入日志
    // logger->log("app", Level::INFO, "应用启动");
    // logger->log("app", Level::DEBUG, "用户 ID：{}", 12345);
    // logger->log("app", Level::WARN, "API 响应慢：{}ms", 500);
    // logger->log("app", Level::ERROR, "数据库连接失败");


    // 直接使用管理类手动刷新
    // logger->flush("app");

    MainWindow w;
    w.show();
    a.exec();

    // 清理
    logger->shutdown();
    return 0;
}
