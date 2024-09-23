#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include <QProcess>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCheckBox>
#include <QTimer>
#include <QFileDialog>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QTabWidget>
#include "NetServer.h"
#include "Expand.h"
#include "AgentTest.h"


//全局字号变量
extern QFont font_s;
extern QFont font_m;
extern QFont font_l;



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

public:
    void updateInitFile();          //更新配置信息
    void loadInitFile();            //读取配置文件
    bool isTimerActive();
    void setSendBuffer(QString str);
    QString getSendBuffer();

public slots:
    void slot_PingBtn();
    void slot_ReadPingOutput();
    void slot_TcpSendConnBtn();
    void slot_UdpSendConnBtn();
    void slot_TcpSendBtn();
    void slot_UdpSendBtn();
    void slot_ClearSendBtn();
    void slot_TcpTimerSendMessage();
    void slot_UdpTimerSendMessage();
    void slot_StopKeepBtn();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override; // 拖动进入事件
    void dropEvent(QDropEvent *event) override;           // 放下事件

private:
    void uiBuild();

private:
    //编辑框
    QLineEdit* m_IPEdit;
    QLineEdit* m_TcpSendPortEdit;
    QLineEdit* m_UdpSendPortEdit;

    //连接按钮
    QPushButton* m_PingBtn;
    QPushButton* m_TcpSendConnBtn;
    QPushButton* m_UdpSendConnBtn;

    //连续发送
    QCheckBox* m_KeepSendBox;
    QLineEdit* m_SendIntervalEdit;
    QTimer* m_TcpTimer;
    QTimer* m_UdpTimer;
    QPushButton* m_stopKeepBtn;

    //发送按钮
    QPushButton* m_TcpSendBtn;
    QPushButton* m_UdpSendBtn;

    //发送区清空按钮
    QPushButton* m_ClearSendBtn;

    //发送区编辑框
    QTextEdit* m_SendTextBuffer;

    //套接字
    QTcpSocket* m_TcpSocket;
    QUdpSocket* m_UdpSocket;
    QHostAddress m_address;

    //选项栏
    QTabWidget* m_tabWidget;
    NetServer* m_NetServer;
    Expand* m_Expand;
    AgentTest* m_AgentTest;

    //ping
    QProcess *m_process;

    //窗口移动
    bool m_isDragging;
    QPoint m_lastMousePos;

};
#endif // MAINWINDOW_H
