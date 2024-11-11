#include "mainwindow.h"
#pragma execution_character_set("utf-8")

QFont font_s("Microsoft YaHei");
QFont font_m("Microsoft YaHei");
QFont font_l("Microsoft YaHei");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1120, 770);
    setWindowIcon(QIcon(":/cover.ico"));
    setAcceptDrops(true);       // 允许拖放文件
    m_isDragging = false;

    //字号设置
    font_s.setPixelSize(15);
    font_m.setPixelSize(17);
    font_l.setPixelSize(20);

    //创建UI界面
    uiBuild();

    //创建套接字
    m_TcpSocket = new QTcpSocket(this);
    m_UdpSocket = new QUdpSocket(this);

    //读取配置文件
    loadInitFile();

    //ping处理
    m_process = new QProcess(this);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &MainWindow::slot_ReadPingOutput);
}


void MainWindow::updateInitFile()
{
    QJsonObject fileJson;
    fileJson.insert("IP地址", m_IPEdit->text());
    fileJson.insert("TCP端口", m_TcpSendPortEdit->text());
    fileJson.insert("UDP端口", m_UdpSendPortEdit->text());
    fileJson.insert("发送间隔", m_SendIntervalEdit->text());
    fileJson.insert("发送缓冲区", m_SendTextBuffer->toPlainText());
    fileJson.insert("TCP接收端口", m_NetServer->getTcpRecvPort());
    fileJson.insert("UDP接收端口", m_NetServer->getUdpRecvPort());
    fileJson.insert("编辑区1", m_Expand->getBufferText(0));
    fileJson.insert("编辑区2", m_Expand->getBufferText(1));
    fileJson.insert("编辑区3", m_Expand->getBufferText(2));
    fileJson.insert("定时器", m_Expand->getTimerText());

    QFile file("./NetInit.ini");                        //定义无名文件对象
    file.open(QIODevice::WriteOnly);                    //读写方式打开文件
    file.write(QString(QJsonDocument(fileJson).toJson()).toUtf8());
    file.close();
}

void MainWindow::loadInitFile()
{
    QFile file("./NetInit.ini");
    file.open(QIODevice::ReadOnly);

    QJsonParseError jsonParseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), &jsonParseError);
    file.close();
    QJsonObject jsonObj = jsonDoc.object();

    m_IPEdit->setText(jsonObj.value("IP地址").toString());
    m_TcpSendPortEdit->setText(jsonObj.value("TCP端口").toString());
    m_UdpSendPortEdit->setText(jsonObj.value("UDP端口").toString());
    m_NetServer->setTcpRecvPort(jsonObj.value("TCP接收端口").toString());
    m_NetServer->setUdpRecvPort(jsonObj.value("UDP接收端口").toString());
    m_SendIntervalEdit->setText(jsonObj.value("发送间隔").toString());
    m_SendTextBuffer->setText(jsonObj.value("发送缓冲区").toString());
    m_Expand->setBufferText1(jsonObj.value("编辑区1").toString());
    m_Expand->setBufferText2(jsonObj.value("编辑区2").toString());
    m_Expand->setBufferText3(jsonObj.value("编辑区3").toString());
    m_Expand->setTimerText(jsonObj.value("定时器").toString());
}

bool MainWindow::isTimerActive()
{
    return m_stopKeepBtn->isEnabled();
}

void MainWindow::setSendBuffer(QString str)
{
    m_SendTextBuffer->setText(str);
}

QString MainWindow::getSendBuffer()
{
    return m_SendTextBuffer->toPlainText();
}

//ping按钮
void MainWindow::slot_PingBtn()
{
    QString program = "ping";            // ping命令
    QStringList arguments;
    arguments << m_IPEdit->text();       // 设置ping的参数
    m_process->start(program, arguments);
    updateInitFile();
}

//ping逐行读取
void MainWindow::slot_ReadPingOutput()
{
    QString output = QString::fromLocal8Bit(m_process->readAllStandardOutput()); // 读取ping的输出
    m_NetServer->setTcpRecvBufferText(output);
}

void MainWindow::slot_TcpSendConnBtn()
{
    static bool Connected = false;
    Connected = !Connected;
    if(Connected)
    {
        m_TcpSocket->connectToHost(m_IPEdit->text(), m_TcpSendPortEdit->text().toInt());
        if(m_TcpSocket->waitForConnected(500))
        {
            m_TcpSendConnBtn->setText("已连接");
            m_TcpSendBtn->setEnabled(true);
            m_TcpSendPortEdit->setEnabled(false);
            m_IPEdit->setEnabled(false);
        }
        else
        {
            m_NetServer->setTcpRecvBufferText("连接失败。");
        }
    }
    else
    {
        slot_StopKeepBtn();
        m_TcpSendConnBtn->setText("连接");
        m_TcpSocket->disconnectFromHost();
        m_TcpSendBtn->setEnabled(false);
        m_TcpSendPortEdit->setEnabled(true);
        if(m_UdpSendPortEdit->isEnabled())
            m_IPEdit->setEnabled(true);
    }
}

void MainWindow::slot_UdpSendConnBtn()
{
    static bool Connected = false;
    Connected = !Connected;
    if(Connected)
    {
        m_address.setAddress(m_IPEdit->text());
        m_UdpSendConnBtn->setText("已连接");
        m_UdpSendBtn->setEnabled(true);
        m_UdpSendPortEdit->setEnabled(false);
        m_IPEdit->setEnabled(false);
    }
    else
    {
        slot_StopKeepBtn();
        m_UdpSendConnBtn->setText("连接");
        m_UdpSendBtn->setEnabled(false);
        m_UdpSendPortEdit->setEnabled(true);
        if(m_TcpSendPortEdit->isEnabled())
            m_IPEdit->setEnabled(true);
    }
}

void MainWindow::slot_TcpSendBtn()
{
    int interval = m_SendIntervalEdit->text().toInt();

    if(m_KeepSendBox->isChecked() && interval >= 100)
    {
        m_TcpTimer->start(interval);
        m_stopKeepBtn->setEnabled(true);
    }

    m_TcpSocket->write(m_SendTextBuffer->toPlainText().toUtf8());
    updateInitFile();
}

void MainWindow::slot_UdpSendBtn()
{
    int interval = m_SendIntervalEdit->text().toInt();

    if(m_KeepSendBox->isChecked() && interval >= 100)
    {
        m_UdpTimer->start(interval);
        m_stopKeepBtn->setEnabled(true);
    }

    m_UdpSocket->writeDatagram(m_SendTextBuffer->toPlainText().toUtf8(), m_address, m_UdpSendPortEdit->text().toInt());
    updateInitFile();
}

void MainWindow::slot_ClearSendBtn()
{
    m_SendTextBuffer->setText("");
}

void MainWindow::slot_TcpTimerSendMessage()
{
    m_TcpSocket->write(m_SendTextBuffer->toPlainText().toUtf8());
}

void MainWindow::slot_UdpTimerSendMessage()
{
    m_UdpSocket->writeDatagram(m_SendTextBuffer->toPlainText().toUtf8(), m_address, m_UdpSendPortEdit->text().toInt());
}

void MainWindow::slot_StopKeepBtn()
{
    m_TcpTimer->stop();
    m_UdpTimer->stop();
    m_stopKeepBtn->setEnabled(false);

    m_Expand->stopTimerClose();
}

//通过鼠标点击事件，识别鼠标左键按下操作，并记录当前位置
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_lastMousePos = event->pos();
        m_isDragging = true;
    }
}

//通过鼠标移动事件，计算鼠标光标的坐标变化，并借助move事件使界面进行相同的移动。如果鼠标没有松开，需要判定鼠标一直在按钮内
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton))
        move(pos() + event->pos() - m_lastMousePos);        // 计算窗口的新位置并移动窗口
}

//鼠标释放事件
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_isDragging = false;               // 鼠标释放，停止拖动
}

//拖放事件
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())                    // 数据中是否包含URL
        event->acceptProposedAction();                  // 如果是则接收动作
    else event->ignore();                               // 否则忽略该事件
}

//拖放事件
void MainWindow::dropEvent(QDropEvent *event)
{
   QList<QUrl> urls = event->mimeData()->urls();
   foreach(QUrl url, urls)
   {
       QString fileName = url.toLocalFile();
       QFile file(fileName);
       if (file.open(QIODevice::ReadOnly | QIODevice::Text))
       {
           QTextStream in(&file);
           m_SendTextBuffer->setText(in.readAll());      // 将文件中所有内容读入编辑器
           file.close();
       }
   }
   event->acceptProposedAction();
}

void MainWindow::uiBuild()
{
    //目的IP
    QLabel* ui_IP = new QLabel(this);
    ui_IP->setGeometry(30, 25, 0, 0);
    ui_IP->setText("IP地址");
    ui_IP->setFont(font_l);
    ui_IP->adjustSize();

    m_IPEdit = new QLineEdit(this);
    m_IPEdit->setGeometry(130, 20, 210, 35);
    m_IPEdit->setFont(font_l);

    m_PingBtn = new QPushButton(this);
    m_PingBtn->setGeometry(370, 20, 100, 35);
    m_PingBtn->setText("Ping");
    m_PingBtn->setFont(font_s);
    connect(m_PingBtn, &QPushButton::clicked, this, &MainWindow::slot_PingBtn);

    //TCP端口号
    QLabel* ui_TcpSendPort = new QLabel(this);
    ui_TcpSendPort->setGeometry(30, 75, 0, 0);
    ui_TcpSendPort->setText("TCP端口");
    ui_TcpSendPort->setFont(font_l);
    ui_TcpSendPort->adjustSize();

    m_TcpSendPortEdit = new QLineEdit(this);
    m_TcpSendPortEdit->setGeometry(130, 70, 210, 35);
    m_TcpSendPortEdit->setFont(font_l);

    m_TcpSendConnBtn = new QPushButton(this);
    m_TcpSendConnBtn->setGeometry(370, 70, 100, 35);
    m_TcpSendConnBtn->setText("连接");
    m_TcpSendConnBtn->setFont(font_s);
    connect(m_TcpSendConnBtn, &QPushButton::clicked, this, &MainWindow::slot_TcpSendConnBtn);

    //UDP端口号
    QLabel* ui_UdpSendPort = new QLabel(this);
    ui_UdpSendPort->setGeometry(30, 125, 0, 0);
    ui_UdpSendPort->setText("UDP端口");
    ui_UdpSendPort->setFont(font_l);
    ui_UdpSendPort->adjustSize();

    m_UdpSendPortEdit = new QLineEdit(this);
    m_UdpSendPortEdit->setGeometry(130, 120, 210, 35);
    m_UdpSendPortEdit->setFont(font_l);

    m_UdpSendConnBtn = new QPushButton(this);
    m_UdpSendConnBtn->setGeometry(370, 120, 100, 35);
    m_UdpSendConnBtn->setText("连接");
    m_UdpSendConnBtn->setFont(font_s);
    connect(m_UdpSendConnBtn, &QPushButton::clicked, this, &MainWindow::slot_UdpSendConnBtn);

    //持续发送
    m_KeepSendBox = new QCheckBox(this);
    m_KeepSendBox->setGeometry(30, 180, 100, 35);
    m_KeepSendBox->setText("持续发送");
    m_KeepSendBox->setFont(font_s);

    m_SendIntervalEdit = new QLineEdit(this);
    m_SendIntervalEdit->setGeometry(30, 220, 60, 30);
    m_SendIntervalEdit->setFont(font_s);

    QLabel* ui_ms = new QLabel(this);
    ui_ms->setGeometry(95, 225, 0, 0);
    ui_ms->setText("ms");
    ui_ms->setFont(font_s);
    ui_ms->adjustSize();

    m_TcpTimer = new QTimer(this);
    connect(m_TcpTimer, &QTimer::timeout, this, &MainWindow::slot_TcpTimerSendMessage);
    m_UdpTimer = new QTimer(this);
    connect(m_UdpTimer, &QTimer::timeout, this, &MainWindow::slot_UdpTimerSendMessage);

    m_stopKeepBtn = new QPushButton(this);
    m_stopKeepBtn->setGeometry(140, 220, 210, 30);
    m_stopKeepBtn->setText("停止发送");
    m_stopKeepBtn->setFont(font_s);
    m_stopKeepBtn->setEnabled(false);
    connect(m_stopKeepBtn, &QPushButton::clicked, this, &MainWindow::slot_StopKeepBtn);

    //发送
    m_TcpSendBtn = new QPushButton(this);
    m_TcpSendBtn->setGeometry(140, 180, 100, 35);
    m_TcpSendBtn->setText("TCP发送 ");
    m_TcpSendBtn->setFont(font_s);
    m_TcpSendBtn->setEnabled(false);
    connect(m_TcpSendBtn, &QPushButton::clicked, this, &MainWindow::slot_TcpSendBtn);

    m_UdpSendBtn = new QPushButton(this);
    m_UdpSendBtn->setGeometry(250, 180, 100, 35);
    m_UdpSendBtn->setText("UDP发送 ");
    m_UdpSendBtn->setFont(font_s);
    m_UdpSendBtn->setEnabled(false);
    connect(m_UdpSendBtn, &QPushButton::clicked, this, &MainWindow::slot_UdpSendBtn);

    //清空发送缓冲区
    m_ClearSendBtn = new QPushButton(this);
    m_ClearSendBtn->setGeometry(370, 220, 100, 30);
    m_ClearSendBtn->setText("清空");
    m_ClearSendBtn->setFont(font_s);
    connect(m_ClearSendBtn, &QPushButton::clicked, this, &MainWindow::slot_ClearSendBtn);

    //发送缓冲区
    m_SendTextBuffer = new QTextEdit(this);
    m_SendTextBuffer->setGeometry(30, 265, 450, 455);
    m_SendTextBuffer->setFont(font_m);

    //选项栏
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setGeometry(600, 20, 450, 700);
    QWidget* server = new QWidget(this);
    QWidget* expand = new QWidget(this);
    QWidget* agent = new QWidget(this);
    server->resize(150, 35);
    expand->resize(150, 35);
    agent->resize(150, 35);
    m_tabWidget->setFont(font_m);
    m_tabWidget->addTab(server, "接收区");
    m_tabWidget->addTab(expand, "扩展");
    m_tabWidget->addTab(agent, "测试");
    m_tabWidget->setObjectName("tabWidget");
    m_tabWidget->setStyleSheet("QTabBar::tab { width: 150px; height: 35px;background-color:transparent;color:rgb(120,120,120);}"
                       "QTabBar::tab:selected{ color:rgb(75,75,110); border-bottom:2px solid#4b4b6e; }"
                       "QTabBar::tab:hover{ color:rgb(0,0,0); }"
                               "QTabWidget:pane{border:0px; background-color:transparent}");

    //选项卡
    m_NetServer = new NetServer(server);
    m_Expand = new Expand(expand);
    m_AgentTest = new AgentTest(agent);
}

