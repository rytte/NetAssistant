#ifndef NETSERVER_H
#define NETSERVER_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QLabel>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTcpServer>
#include <QHostAddress>

class NetServer : public QWidget
{
public:
    NetServer(QWidget *parent = nullptr);

    void setTcpRecvBufferText(QString data) {m_TcpRecvBuffer->append(data);}

public slots:
    void slot_TcpRecvConnBtn();
    void slot_UdpRecvConnBtn();
    void slot_NewConnect();
    void slot_RecvTcpData();
    void slot_RecvUdpData();

private:
    void uiBuild();

private:

    //套接字
    QTcpServer* m_TcpServer;
    QTcpSocket* m_TcpSocket;
    QUdpSocket* m_UdpSocket;

    //接收端口
    QLineEdit* m_TcpRecvEdit;
    QLineEdit* m_UdpRecvEdit;

    //连接按钮
    QPushButton* m_TcpRecvConnBtn;
    QPushButton* m_UdpRecvConnBtn;

    //接收缓冲区
    QTextBrowser* m_TcpRecvBuffer;
    QTextBrowser* m_UdpRecvBuffer;
};

#endif // NETSERVER_H
