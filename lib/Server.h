/*
 * @File      : Server.h
 * Author     : CodyLi
 * Created    : 2023/11/4
 * Project    : Simple2DServer
 * Platform   : Windows11+Qt6+C++11
 * Description: TCP服务端
 */

#ifndef SIMPLE2DSERVER_SERVER_H
#define SIMPLE2DSERVER_SERVER_H

#include <iostream>
#include <string>
#include <map>
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QPainter>
#include <QPaintEvent>

#define NAME_HAS_EXIST   "\033[31mThe Entity Name[%1] Already Exists!\033[0m"
#define NAME_NOT_FOUND   "\033[31mThe Entity Name[%1] Not Found!\033[0m"
#define COMMAND_INVALID  "\033[31mThe Format of Command is Invalid!\033[0m"
#define EXECUTE_SUCCESS  "\033[32mExecute Command Successfully!\033[0m"

#define COMMAND_INSERT   "insert"
#define COMMAND_MOVE     "move"
#define COMMAND_PAINT    "paint"
#define ENTITY_CIRCLE    "circle"
#define ENTITY_RECTANGLE "rectangle"

struct Entity {
    /*
     * Entity: 图形实体
     * x, y: 位置
     * width, height: 宽度和高度
     * radius: 直径
     * name: 名称
     * color: 填充色(默认不填充)
     */
    double x;
    double y;
    double width;
    double height;
    double radius;
    QString name;
    Qt::GlobalColor color;

    // 初始化空对象
    Entity() : x(0), y(0), width(0), height(0), radius(0), name(""), color(Qt::white) {}
    // 初始化矩形
    Entity(double x, double y, double width, double height, QString name) :
            x(x), y(y), width(width), height(height), radius(0), name(std::move(name)), color(Qt::white) {}
    // 初始化圆形
    Entity(double x, double y, double radius, QString name) :
            x(x), y(y), width(0), height(0), radius(radius), name(std::move(name)), color(Qt::white) {}

    // 重载比较运算符
    bool operator==(const Entity &rhs) const {
        return x == rhs.x &&
               y == rhs.y &&
               width == rhs.width &&
               height == rhs.height &&
               radius == rhs.radius &&
               name == rhs.name &&
               color == rhs.color;
    }

    bool operator!=(const Entity &rhs) const {
        return !(rhs == *this);
    }
};


QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QMainWindow {
Q_OBJECT

public:
    explicit Server(QWidget *parent = nullptr);
    ~Server() override;
    void StartServer(const QHostAddress& _ip, int _port);

private:
    Ui::Server *ui{};

private:
    std::map<QString, Entity> EntityMap;
    Entity CurrentEntity;
    std::unordered_map<QString , Qt::GlobalColor> ColorMap;
    static void DrawCircle(QPainter &Painter, Entity &item);
    static void DrawRectangle(QPainter &Painter, Entity &item);

private:
    QTcpServer *tcpServer = nullptr;
    QTcpSocket *tcpSocket = nullptr;

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void ClientConnect();
    void ListenMessage();
    QString ParseCommand(const QString& command);

private slots:
    void InsertEntity(Entity &item);
    void MoveEntity(const QString& name, int offset_x, int offset_y);
    void PaintEntity(const QString& name, Qt::GlobalColor color);
};


#endif //SIMPLE2DSERVER_SERVER_H
