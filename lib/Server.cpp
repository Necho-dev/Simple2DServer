#include "Server.h"
#include "ui_Server.h"

Server::Server(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::Server) {
    ui->setupUi(this);

    // 初始化颜色映射
    ColorMap["black"] = Qt::black;
    ColorMap["red"] = Qt::red;
}

Server::~Server() {
    delete ui;
    tcpServer->close();
    tcpServer->deleteLater();
    if (tcpSocket) {
        tcpSocket->close();
        tcpSocket->deleteLater();
        delete this->tcpSocket;
    }
}

void Server::StartServer(const QHostAddress& _ip, int _port) {
    // 初始化服务器
    tcpServer = new QTcpServer(this);
    // 开始监听端口
    if (!tcpServer->listen(_ip, _port)) {
        qDebug("\033[31m[ERROR] Sever Failed to Start!\033[0m");
    } else {
        qDebug("\033[33m[WAIT] Server is Listening on [%s:%d]\033[0m", qPrintable(tcpServer->serverAddress().toString()), tcpServer->serverPort());
    }
    // 等待客户端连接
    connect(tcpServer,&QTcpServer::newConnection,this,&Server::ClientConnect);
}

void Server::ClientConnect() {
    // 获取客户端连接
    while (tcpServer->hasPendingConnections()) {
        // 通过Socket进行通信
        tcpSocket = tcpServer->nextPendingConnection();
        qDebug("\033[32m[OK] Client [%s:%d] Connected!\033[0m", qPrintable(tcpSocket->peerAddress().toString()), tcpSocket->peerPort());
        // 向客户端发送欢迎信息
        tcpSocket->write("\033[32mWelcome to Simple2DServer!\033[0m");
        // 监听客户端发送的指令
        connect(tcpSocket, &QTcpSocket::readyRead, this, &Server::ListenMessage);
    }
}

void Server::ListenMessage() {
    // 获取客户端发送的指令
    QString command = tcpSocket->readAll();
    qDebug("[CLIENT] @%d: %s", tcpSocket->peerPort(), qPrintable(command));
    if (command == "exit") {
        // 结束指令
        qDebug("\033[33m[FINISH] Client [%s:%d] Disconnected!\033[0m", qPrintable(tcpSocket->peerAddress().toString()), tcpSocket->peerPort());
        // 结束通信 关闭Socket
        tcpSocket->disconnected();
        tcpSocket->close();
        return;
    }
    // 根据指令执行操作
    QString response = ParseCommand(command);
    // 向客户端发送响应
    tcpSocket->write(response.toUtf8());
}

QString Server::ParseCommand(const QString &command) {
    QStringList params = command.split(" ");
    // 指令: Insert
    if (params[0] == COMMAND_INSERT) {
        // 检查参数个数是否正确 不正确则报错
        if (params[1] == ENTITY_CIRCLE && params.size() == 6) {
            // 检查名称是否已存在 存在则报错
            if (EntityMap[params[5]] != Entity()) {
                return QString(NAME_HAS_EXIST).arg(params[5]);
            }
            Entity a{params[2].toDouble(), params[3].toDouble(), params[4].toDouble(), params[5]};
            InsertEntity(a);
        } else if (params[1] == ENTITY_RECTANGLE && params.size() == 7) {
            // 检查名称是否已存在 存在则报错
            if (EntityMap[params[6]] != Entity()) {
                return QString(NAME_HAS_EXIST).arg(params[6]);
            }
            Entity e{params[2].toDouble(), params[3].toDouble(),params[4].toDouble(),
                     params[5].toDouble(),params[6]};
            InsertEntity(e);
        } else {
            return COMMAND_INVALID;
        }
    } else if (params[0] == COMMAND_MOVE) {
        // 检查参数个数是否正确 不正确则报错
        if (params.size() == 4) {
            // 检查名称是否存在 不存在则报错
            if (EntityMap[params[1]] == Entity()) {
                return QString(NAME_NOT_FOUND).arg(params[1]);
            }
            MoveEntity(params[1], params[2].toInt(), params[3].toInt());
        } else {
            return COMMAND_INVALID;
        }
    } else if (params[0] == COMMAND_PAINT) {
        // 检查参数个数是否正确 不正确则报错
        if (params.size() == 3) {
            // 检查名称是否已存在 不存在则报错
            if (EntityMap[params[1]] == Entity()) {
                return QString(NAME_NOT_FOUND).arg(params[1]);
            }
            PaintEntity(params[1], ColorMap[params[2]]);
        } else {
            return COMMAND_INVALID;
        }
    }
    // 指令执行成功
    return EXECUTE_SUCCESS;
}

void Server::paintEvent(QPaintEvent *event) {
    // 创建QPainter对象
    QPainter Painter(this);
    // 设置图形防锯齿(反走样)
    Painter.setRenderHint(QPainter::Antialiasing);
    // 设置文字防锯齿(反走样)
    Painter.setRenderHint(QPainter::TextAntialiasing);
    // 存量绘图操作
    for (auto &item : EntityMap) {
        if (item.second.radius != 0) {
            DrawCircle(Painter, item.second);
        } else {
            DrawRectangle(Painter, item.second);
        }
    }

    // 结束绘图操作
    Painter.end();
}

void Server::InsertEntity(Entity &item) {
    EntityMap[item.name] = item;

    update();
}

void Server::MoveEntity(const QString& name, int offset_x, int offset_y) {
    EntityMap[name].x += offset_x;
    EntityMap[name].y += offset_y;

    update();
}

void Server::PaintEntity(const QString& name, Qt::GlobalColor color) {
    EntityMap[name].color = color;

    update();
}

void Server::DrawCircle(QPainter &Painter, Entity &item) {
    int x = static_cast<int>(item.x);
    int y = static_cast<int>(item.y);
    int radius = static_cast<int>(item.radius);

    // 设置画笔
    QPen Pen(Qt::black, 1, Qt::SolidLine);
    Painter.setPen(Pen);
    // 设置画刷
    if (item.color == Qt::white) {
        Painter.setBrush(Qt::NoBrush);
    } else {
        QBrush Brush(item.color, Qt::SolidPattern);
        Painter.setBrush(Brush);
    }
    // 绘制圆形
    Painter.drawEllipse(x, y, radius, radius);

    // 创建字体
    QFont font("Arial", 10);
    Painter.setFont(font);
    // 获取字体测量信息
    QFontMetrics metrics(font);
    QString text = item.name + ": Radius = " + QString::number(item.radius);
    // 计算文本的宽度和高度
    int textWidth = metrics.horizontalAdvance(text);
    int textHeight = metrics.height();
    // 绘制文字
    Painter.drawText(x + radius, y, textWidth, textHeight,
                     Qt::AlignLeft | Qt::AlignVCenter, text);
}

void Server::DrawRectangle(QPainter &Painter, Entity &item) {
    int x = static_cast<int>(item.x);
    int y = static_cast<int>(item.y);
    int width = static_cast<int>(item.width);
    int height = static_cast<int>(item.height);

    // 设置画笔
    QPen Pen(Qt::black, 1, Qt::SolidLine);
    Painter.setPen(Pen);
    // 设置画刷
    if (item.color == Qt::white) {
        Painter.setBrush(Qt::NoBrush);
    } else {
        QBrush Brush(item.color, Qt::SolidPattern);
        Painter.setBrush(Brush);
    }
    // 绘制矩形
    Painter.drawRect(x, y, width, height);

    // 创建字体
    QFont font("Arial", 10);
    Painter.setFont(font);
    // 获取字体测量信息
    QFontMetrics metrics(font);
    QString text = item.name + ": " + QString::number(width) + " × " + QString::number(height);
    // 计算文本的宽度和高度
    int textWidth = metrics.horizontalAdvance(text);
    int textHeight = metrics.height();
    // 绘制文字
    Painter.drawText(x, y - textHeight - 5, textWidth, textHeight,
                     Qt::AlignLeft | Qt::AlignVCenter, text);
}
