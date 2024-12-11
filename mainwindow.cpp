#include "mainwindow.h"
#pragma execution_character_set("utf-8")

QFont font_s("Microsoft YaHei");
QFont font_m("Microsoft YaHei");
QFont font_l("Microsoft YaHei");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(840, 580);
    setWindowIcon(QIcon(":/cover.ico"));
    setAcceptDrops(true);       // 允许拖放文件
    m_isDragging = false;

    //字号设置
    font_s.setPixelSize(11);
    font_m.setPixelSize(12);
    font_l.setPixelSize(14);

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

    m_address.setAddress(m_IPEdit->text());
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
    QLabel* ui_IP = new QLabel("IP地址", this);
    ui_IP->setFont(font_l);

    m_IPEdit = new QLineEdit(this);
    m_IPEdit->setFixedSize(150, 25);
    m_IPEdit->setFont(font_l);

    m_PingBtn = new QPushButton("Ping", this);
    m_PingBtn->setFixedSize(75, 25);
    m_PingBtn->setFont(font_s);
    connect(m_PingBtn, &QPushButton::clicked, this, &MainWindow::slot_PingBtn);

    //TCP端口号
    QLabel* ui_TcpSendPort = new QLabel("TCP端口", this);
    ui_TcpSendPort->setFont(font_l);

    m_TcpSendPortEdit = new QLineEdit(this);
    m_TcpSendPortEdit->setFixedSize(150, 25);
    m_TcpSendPortEdit->setFont(font_l);

    m_TcpSendConnBtn = new QPushButton("连接", this);
    m_TcpSendConnBtn->setFixedSize(75, 25);
    m_TcpSendConnBtn->setFont(font_s);
    connect(m_TcpSendConnBtn, &QPushButton::clicked, this, &MainWindow::slot_TcpSendConnBtn);

    //UDP端口号
    QLabel* ui_UdpSendPort = new QLabel("UDP端口", this);
    ui_UdpSendPort->setFont(font_l);

    m_UdpSendPortEdit = new QLineEdit(this);
    m_UdpSendPortEdit->setFixedSize(150, 25);
    m_UdpSendPortEdit->setFont(font_l);

    //持续发送
    m_KeepSendBox = new QCheckBox("持续发送", this);
    m_KeepSendBox->setFixedSize(75, 22);
    m_KeepSendBox->setFont(font_s);

    m_SendIntervalEdit = new QLineEdit(this);
    m_SendIntervalEdit->setFixedSize(40, 22);
    m_SendIntervalEdit->setFont(font_s);

    QLabel* ui_ms = new QLabel("ms", this);
    ui_ms->setFont(font_s);
    ui_ms->adjustSize();

    m_TcpTimer = new QTimer(this);
    connect(m_TcpTimer, &QTimer::timeout, this, &MainWindow::slot_TcpTimerSendMessage);
    m_UdpTimer = new QTimer(this);
    connect(m_UdpTimer, &QTimer::timeout, this, &MainWindow::slot_UdpTimerSendMessage);

    m_stopKeepBtn = new QPushButton("停止发送", this);
    m_stopKeepBtn->setFixedSize(156, 22);
    m_stopKeepBtn->setFont(font_s);
    m_stopKeepBtn->setEnabled(false);
    connect(m_stopKeepBtn, &QPushButton::clicked, this, &MainWindow::slot_StopKeepBtn);

    //发送
    m_TcpSendBtn = new QPushButton("TCP发送", this);
    m_TcpSendBtn->setFixedSize(75, 22);
    m_TcpSendBtn->setFont(font_s);
    m_TcpSendBtn->setEnabled(false);
    connect(m_TcpSendBtn, &QPushButton::clicked, this, &MainWindow::slot_TcpSendBtn);

    m_UdpSendBtn = new QPushButton("UDP发送", this);
    m_UdpSendBtn->setFixedSize(75, 22);
    m_UdpSendBtn->setFont(font_s);
    connect(m_UdpSendBtn, &QPushButton::clicked, this, &MainWindow::slot_UdpSendBtn);

    //清空发送缓冲区
    m_ClearSendBtn = new QPushButton("清空", this);
    m_ClearSendBtn->setFixedSize(75, 22);
    m_ClearSendBtn->setFont(font_s);
    connect(m_ClearSendBtn, &QPushButton::clicked, this, &MainWindow::slot_ClearSendBtn);

    //发送缓冲区
    m_SendTextBuffer = new QTextEdit(this);
    m_SendTextBuffer->setFixedSize(340, 340);
    m_SendTextBuffer->setFont(font_m);

    //选项栏
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setFixedSize(340, 530);
    QWidget* server = new QWidget(this);
    QWidget* expand = new QWidget(this);
    QWidget* agent = new QWidget(this);
    m_tabWidget->setFont(font_m);
    m_tabWidget->addTab(server, "接收区");
    m_tabWidget->addTab(expand, "扩展");
    m_tabWidget->addTab(agent, "测试");
    m_tabWidget->setObjectName("tabWidget");
    m_tabWidget->setStyleSheet("QTabBar::tab { width: 110px; height: 25px;background-color:transparent;color:rgb(120,120,120);}"
                       "QTabBar::tab:selected{ color:rgb(75,75,110); border-bottom:2px solid#4b4b6e; }"
                       "QTabBar::tab:hover{ color:rgb(0,0,0); }"
                               "QTabWidget:pane{border:0px; background-color:transparent}");

    //选项卡
    m_NetServer = new NetServer(server);
    m_Expand = new Expand(expand);
    m_AgentTest = new AgentTest(agent);


    //布局管理器
    QWidget* widget = new QWidget(this);
    widget->setFixedSize(340, 530);
    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->addWidget(ui_IP, 0, 0);
    gridLayout->addWidget(m_IPEdit, 0, 1);
    gridLayout->addWidget(m_PingBtn, 0, 2);
    gridLayout->addWidget(ui_TcpSendPort, 1, 0);
    gridLayout->addWidget(m_TcpSendPortEdit, 1, 1);
    gridLayout->addWidget(m_TcpSendConnBtn, 1, 2);
    gridLayout->addWidget(ui_UdpSendPort, 2, 0);
    gridLayout->addWidget(m_UdpSendPortEdit, 2, 1);
    gridLayout->addWidget(new QWidget, 2, 2);
    gridLayout->setHorizontalSpacing(20);
    gridLayout->setVerticalSpacing(10);

    QHBoxLayout* hBox1 = new QHBoxLayout;
    hBox1->addWidget(m_KeepSendBox);
    hBox1->addWidget(m_TcpSendBtn);
    hBox1->addWidget(m_UdpSendBtn);
    hBox1->addWidget(new QWidget);

    QHBoxLayout* hBox2 = new QHBoxLayout;
    hBox2->addWidget(m_SendIntervalEdit);
    hBox2->addWidget(ui_ms);
    hBox2->addWidget(m_stopKeepBtn);
    hBox2->addSpacing(10);
    hBox2->addWidget(m_ClearSendBtn);

    QVBoxLayout* vBox = new QVBoxLayout;
    vBox->addLayout(gridLayout);
    vBox->addSpacing(15);
    vBox->addLayout(hBox1);
    vBox->addLayout(hBox2);
    vBox->addSpacing(10);
    vBox->addWidget(m_SendTextBuffer);

    widget->setLayout(vBox);

    QHBoxLayout* hBox3 = new QHBoxLayout;
    hBox3->addWidget(widget);
    hBox3->addSpacing(60);
    hBox3->addWidget(m_tabWidget);

    //设置布局
    this->setCentralWidget(new QWidget);
    this->centralWidget()->setLayout(hBox3);
}

