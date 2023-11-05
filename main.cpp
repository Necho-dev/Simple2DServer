#include <QApplication>
#include "lib/Server.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // 检查命令行参数
    if (argc > 1) {
        // 获取端口号
        int PortValue = QString(argv[1]).toInt();
        // 检查端口号是否合法
        if (PortValue < 0 || PortValue > 65535) {
            // 端口号不合法
            qDebug("\033[31m [ERROR] The Port[%d] is Invalid!\033[0m", PortValue);
            return 0;
        }
        // 启动服务器
        Server server;
        server.show();
        // 启动服务器
        server.StartServer(QHostAddress("127.0.0.1"), PortValue);
        return QApplication::exec();
    }
    // 使用默认端口号启动服务器
    Server server;
    server.show();
    // 启动服务器
    server.StartServer(QHostAddress("127.0.0.1"), 3000);
    return QApplication::exec();
}
