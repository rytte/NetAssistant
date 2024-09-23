#include "NetServer.h"
#pragma execution_character_set("utf-8")
#include "mainwindow.h"


NetServer::NetServer(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(0, 15, 450, 650);
    m_myParent = (MainWindow* )topLevelWidget();

    //创建UI界面
    uiBuild();

    //TCP服务器
    m_TcpServer = new QTcpServer(this);
    connect(m_TcpServer, &QTcpServer::newConnection, this, &NetServer::slot_NewConnect);       //连接读取函数
    m_TcpSocket = new QTcpSocket(this);

    //UDP套接字
    m_UdpSocket = new QUdpSocket(this);
    connect(m_UdpSocket, &QUdpSocket::readyRead, this, &NetServer::slot_RecvUdpData);       //连接读取函数
}

void NetServer::setTcpRecvPort(QString port)
{
    m_TcpRecvEdit->setText(port);
}

void NetServer::setUdpRecvPort(QString port)
{
    m_UdpRecvEdit->setText(port);
}

void NetServer::setTcpRecvBufferText(QString data)
{
    m_TcpRecvBuffer->append(data);
}

//TCP端口绑定
void NetServer::slot_TcpRecvConnBtn()
{
    static bool Connect = false;
    Connect = !Connect;
    if(Connect)
    {
        m_myParent->updateInitFile();

        m_TcpServer->listen(QHostAddress("0.0.0.0"), m_TcpRecvEdit->text().toInt());

        m_TcpRecvConnBtn->setText("已连接");
        m_TcpRecvEdit->setEnabled(false);
    }
    else
    {
        m_TcpServer->close();
        m_TcpSocket->close();
        m_TcpRecvConnBtn->setText("连接");
        m_TcpRecvEdit->setEnabled(true);
    }
}

//UDP端口绑定
void NetServer::slot_UdpRecvConnBtn()
{
    static bool Connect = false;
    Connect = !Connect;
    if(Connect)
    {
        m_myParent->updateInitFile();

        m_UdpSocket->bind(QHostAddress("0.0.0.0"), m_UdpRecvEdit->text().toInt());

        m_UdpRecvConnBtn->setText("已连接");
        m_UdpRecvEdit->setEnabled(false);
    }
    else
    {
        m_UdpSocket->close();
        m_UdpRecvConnBtn->setText("连接");
        m_UdpRecvEdit->setEnabled(true);
    }
}

//TCP新客户端连接
void NetServer::slot_NewConnect()
{
    m_TcpSocket = m_TcpServer->nextPendingConnection();                                     //获取套接字
    connect(m_TcpSocket, &QTcpSocket::readyRead, this, &NetServer::slot_RecvTcpData);       //连接读取函数
    QString ip = m_TcpSocket->peerAddress().toString();                            //获取ip地址
    QString port = QString("%1").arg(m_TcpSocket->peerPort());                     //获取端口号
    m_TcpRecvBuffer->append(QString("新连接 : IP " + ip + ", Port " + port));
}

//接收TCP消息
void NetServer::slot_RecvTcpData()
{
    QString data = m_TcpSocket->readAll();
    m_TcpRecvBuffer->append(data);
}

//接收UDP消息
void NetServer::slot_RecvUdpData()
{
    QByteArray data;
    data.resize(m_UdpSocket->pendingDatagramSize());
    QHostAddress addr;
    quint16 port;
    m_UdpSocket->readDatagram(data.data(),data.size(),&addr,&port);
    QString str = QString("————————" + addr.toString() + " : " + QString::number(port) + "————————\n");
    m_UdpRecvBuffer->append(str + data);
}

void NetServer::uiBuild()
{
    //TCP接收端口
    QLabel* ui_TcpRecvPort = new QLabel(this);
    ui_TcpRecvPort->setGeometry(0, 5, 0, 0);
    ui_TcpRecvPort->setText("TCP接收端口");
    ui_TcpRecvPort->setFont(font_l);
    ui_TcpRecvPort->adjustSize();

    m_TcpRecvEdit = new QLineEdit(this);
    m_TcpRecvEdit->setGeometry(140, 0, 180, 35);
    m_TcpRecvEdit->setFont(font_l);

    m_TcpRecvConnBtn = new QPushButton(this);
    m_TcpRecvConnBtn->setGeometry(350, 0, 100, 35);
    m_TcpRecvConnBtn->setText("连接");
    m_TcpRecvConnBtn->setFont(font_s);
    connect(m_TcpRecvConnBtn, &QPushButton::clicked, this, &NetServer::slot_TcpRecvConnBtn);

    //TCP接收缓冲区
    m_TcpRecvBuffer = new QTextBrowser(this);
    m_TcpRecvBuffer->setGeometry(0, 50, 450, 265);
    m_TcpRecvBuffer->setFont(font_s);
    m_TcpRecvBuffer->document()->setMaximumBlockCount(1000);

    //UDP接收端口
    QLabel* ui_UdpRecvPort = new QLabel(this);
    ui_UdpRecvPort->setGeometry(0, 340, 0, 0);
    ui_UdpRecvPort->setText("UDP接收端口");
    ui_UdpRecvPort->setFont(font_l);
    ui_UdpRecvPort->adjustSize();

    m_UdpRecvEdit = new QLineEdit(this);
    m_UdpRecvEdit->setGeometry(140, 335, 180, 35);
    m_UdpRecvEdit->setFont(font_l);

    m_UdpRecvConnBtn = new QPushButton(this);
    m_UdpRecvConnBtn->setGeometry(350, 335, 100, 35);
    m_UdpRecvConnBtn->setText("连接");
    m_UdpRecvConnBtn->setFont(font_s);
    connect(m_UdpRecvConnBtn, &QPushButton::clicked, this, &NetServer::slot_UdpRecvConnBtn);

    //UDP接收缓冲区
    m_UdpRecvBuffer = new QTextBrowser(this);
    m_UdpRecvBuffer->setGeometry(0, 380, 450, 265);
    m_UdpRecvBuffer->setFont(font_s);
    m_UdpRecvBuffer->document()->setMaximumBlockCount(1000);
}
