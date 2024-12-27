#include "mainwindow.h"
#pragma execution_character_set("utf-8")

QFont font_s("Microsoft YaHei");
QFont font_m("Microsoft YaHei");
QFont font_l("Microsoft YaHei");

QSettings settings("Yuyi", "NetAssistant");

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
}

void MainWindow::slot_TcpSendConnBtn()
{
    static bool Connected = false;
    if(Connected)
    {
        slot_StopKeepBtn();
        m_TcpSendConnBtn->setText("连接");
        m_TcpSocket->disconnectFromHost();
        m_TcpSendBtn->setEnabled(false);
        m_TcpSendPortEdit->setEnabled(true);
        m_IPEdit->setEnabled(true);
        Connected = false;
    }
    else
    {
        m_TcpSocket->connectToHost(m_IPEdit->text(), m_TcpSendPortEdit->text().toInt());
        if(m_TcpSocket->waitForConnected(500))
        {
            m_TcpSendConnBtn->setText("已连接");
            m_TcpSendBtn->setEnabled(true);
            m_TcpSendPortEdit->setEnabled(false);
            m_IPEdit->setEnabled(false);
            Connected = true;
        }
        else
        {
            m_NetServer->setTcpRecvBufferText("连接失败。");
            Connected = false;
        }
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

    //ping处理
    QProcess *process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardOutput, this,
            [&]{
        QString output = QString::fromLocal8Bit(process->readAllStandardOutput());
        m_NetServer->setTcpRecvBufferText(output);
    });

    QPushButton* pingBtn = new QPushButton("Ping", this);
    pingBtn->setFixedSize(75, 25);
    pingBtn->setFont(font_s);
    connect(pingBtn, &QPushButton::clicked, this, [&]{process->start("ping", QStringList(m_IPEdit->text()));});

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
    connect(m_TcpTimer, &QTimer::timeout, this, [this]{m_TcpSocket->write(m_SendTextBuffer->toPlainText().toUtf8());});
    m_UdpTimer = new QTimer(this);
    connect(m_UdpTimer, &QTimer::timeout, this, [this]{m_UdpSocket->writeDatagram(m_SendTextBuffer->toPlainText().toUtf8(), m_address, m_UdpSendPortEdit->text().toInt());});

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
    QPushButton* clearSendBtn = new QPushButton("清空", this);
    clearSendBtn->setFixedSize(75, 22);
    clearSendBtn->setFont(font_s);
    connect(clearSendBtn, &QPushButton::clicked, this, [this]{m_SendTextBuffer->setText("");});

    //发送缓冲区
    m_SendTextBuffer = new QTextEdit(this);
    m_SendTextBuffer->setFixedSize(340, 340);
    m_SendTextBuffer->setFont(font_m);

    //选项栏
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setFixedSize(340, 530);
    QWidget* server = new QWidget(this);
    QWidget* expand = new QWidget(this);
    QWidget* openai = new QWidget(this);
    QWidget* agent = new QWidget(this);
    m_tabWidget->setFont(font_m);
    m_tabWidget->addTab(server, "接收区");
    m_tabWidget->addTab(expand, "扩展");
    m_tabWidget->addTab(openai, "openai");
    m_tabWidget->addTab(agent, "测试");
    m_tabWidget->setObjectName("tabWidget");
    m_tabWidget->setStyleSheet("QTabBar::tab { width: 85px; height: 25px;background-color:transparent;color:rgb(120,120,120);}"
                       "QTabBar::tab:selected{ color:rgb(75,75,110); border-bottom:2px solid#4b4b6e; }"
                       "QTabBar::tab:hover{ color:rgb(0,0,0); }"
                               "QTabWidget:pane{border:0px; background-color:transparent}");

    //选项卡
    m_NetServer = new NetServer(server);
    m_Expand = new Expand(expand);
    m_OpenAIClient = new OpenAIClient(openai);
    m_AgentTest = new AgentTest(agent);


    //配置文件
    m_IPEdit->setText(settings.value("IP地址").toString());
    m_TcpSendPortEdit->setText(settings.value("TCP端口").toString());
    m_UdpSendPortEdit->setText(settings.value("UDP端口").toString());
    m_SendIntervalEdit->setText(settings.value("发送间隔").toString());
    m_SendTextBuffer->setPlainText(settings.value("发送缓冲区").toString());
    connect(m_IPEdit, &QLineEdit::textChanged, this, [&]{settings.setValue("IP地址", m_IPEdit->text());});
    connect(m_TcpSendPortEdit, &QLineEdit::textChanged, this, [&]{settings.setValue("TCP端口", m_TcpSendPortEdit->text());});
    connect(m_UdpSendPortEdit, &QLineEdit::textChanged, this, [&]{settings.setValue("UDP端口", m_UdpSendPortEdit->text());});
    connect(m_SendIntervalEdit, &QLineEdit::textChanged, this, [&]{settings.setValue("发送间隔", m_SendIntervalEdit->text());});
    connect(m_SendTextBuffer, &QTextEdit::textChanged, this, [&]{settings.setValue("发送缓冲区", m_SendTextBuffer->toPlainText());});


    //布局管理器
    QWidget* widget = new QWidget(this);
    widget->setFixedSize(340, 530);
    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->addWidget(ui_IP, 0, 0);
    gridLayout->addWidget(m_IPEdit, 0, 1);
    gridLayout->addWidget(pingBtn, 0, 2);
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
    hBox2->addWidget(clearSendBtn);

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

