#include "NetServer.h"
#pragma execution_character_set("utf-8")
#include "mainwindow.h"


NetServer::NetServer(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(0, 15, 340, 495);
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
    QString str = QString("—————— From " + addr.toString() + " : " + QString::number(port) + " ——————\n");
    m_UdpRecvBuffer->append(str + data);
}

void NetServer::uiBuild()
{
    //TCP接收端口
    QLabel* ui_TcpRecvPort = new QLabel("TCP接收端口", this);
    ui_TcpRecvPort->setFont(font_l);
    ui_TcpRecvPort->adjustSize();

    m_TcpRecvEdit = new QLineEdit(this);
    m_TcpRecvEdit->setFont(font_l);

    m_TcpRecvConnBtn = new QPushButton("连接", this);
    m_TcpRecvConnBtn->setFont(font_s);
    connect(m_TcpRecvConnBtn, &QPushButton::clicked, this, &NetServer::slot_TcpRecvConnBtn);

    //TCP接收缓冲区
    m_TcpRecvBuffer = new QTextBrowser(this);
    m_TcpRecvBuffer->setFixedSize(340, 195);
    m_TcpRecvBuffer->setFont(font_s);
    m_TcpRecvBuffer->document()->setMaximumBlockCount(1000);

    //UDP接收端口
    QLabel* ui_UdpRecvPort = new QLabel("UDP接收端口", this);
    ui_UdpRecvPort->setFont(font_l);
    ui_UdpRecvPort->adjustSize();

    m_UdpRecvEdit = new QLineEdit(this);
    m_UdpRecvEdit->setFont(font_l);

    m_UdpRecvConnBtn = new QPushButton("连接", this);
    m_UdpRecvConnBtn->setFont(font_s);
    connect(m_UdpRecvConnBtn, &QPushButton::clicked, this, &NetServer::slot_UdpRecvConnBtn);

    //UDP接收缓冲区
    m_UdpRecvBuffer = new QTextBrowser(this);
    m_UdpRecvBuffer->setFixedSize(340, 195);
    m_UdpRecvBuffer->setFont(font_s);
    m_UdpRecvBuffer->document()->setMaximumBlockCount(1000);


    //布局管理器
    QHBoxLayout* hBox1 = new QHBoxLayout;
    hBox1->addWidget(ui_TcpRecvPort);
    hBox1->addWidget(m_TcpRecvEdit);
    hBox1->addWidget(m_TcpRecvConnBtn);
    hBox1->setSpacing(20);

    QHBoxLayout* hBox2 = new QHBoxLayout;
    hBox2->addWidget(ui_UdpRecvPort);
    hBox2->addWidget(m_UdpRecvEdit);
    hBox2->addWidget(m_UdpRecvConnBtn);
    hBox2->setSpacing(20);

    QVBoxLayout* vBox = new QVBoxLayout;
    vBox->addLayout(hBox1);
    vBox->addWidget(m_TcpRecvBuffer);
    vBox->addSpacing(10);
    vBox->addLayout(hBox2);
    vBox->addWidget(m_UdpRecvBuffer);

    this->setLayout(vBox);
}
