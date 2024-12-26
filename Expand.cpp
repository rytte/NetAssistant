#include "Expand.h"
#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include <QDateTime>


Expand::Expand(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(0, 15, 340, 495);

    //获取父控件对象
    m_myParent = (MainWindow* )topLevelWidget();

    //创建UI界面
    uiBuild();
}

//关闭定时器
void Expand::stopTimerClose()
{
    if(m_stopTimer->isActive())
    {
        m_stopTimer->stop();
        m_stopTimerBtn->setText("开启");
        m_timeEdit->setEnabled(true);

        m_countDownTimer->stop();
        m_countDownLabel->setText("");
    }
}

void Expand::slot_StopTimerBtn()
{
    static bool Started = false;
    Started = !Started;
    if(Started)
    {
        if(m_myParent->isTimerActive())
        {
            m_secTime = m_timeEdit->text().toDouble()*60;
            if(m_secTime > 0 && m_secTime <= 360*60)
            {
                m_stopTimer->start(m_secTime*1000);
                m_stopTimerBtn->setText("已开启");
                m_timeEdit->setEnabled(false);
                m_countDownTimer->start(1000);
            }
        }
    }
    else
    {
        stopTimerClose();
    }
}

void Expand::slot_ReplaceBufferBtn()
{
    int bufferIndex = m_tabWidget->currentIndex();

    if(m_textEdit[bufferIndex]->toPlainText() != nullptr)
    {
        m_myParent->setSendBuffer(m_textEdit[bufferIndex]->toPlainText());
    }
}

void Expand::slot_FileSelectBtn()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择文件", "./");
    m_fileEdit->setText(filePath);
}

void Expand::slot_FileReadBtn()
{
    QFile readFile(m_fileEdit->text());
    readFile.open(QIODevice::ReadOnly);
    m_myParent->setSendBuffer(QString::fromUtf8(readFile.readAll()));
    readFile.close();
}

void Expand::uiBuild()
{
    //定时器
    m_stopTimer = new QTimer(this);
    connect(m_stopTimer, &QTimer::timeout, m_myParent, &MainWindow::slot_StopKeepBtn);

    QLabel* ui_timer = new QLabel("定时器：", this);
    ui_timer->setFont(font_m);
    ui_timer->adjustSize();

    m_timeEdit = new QLineEdit(this);
    m_timeEdit->setFont(font_s);
    m_timeEdit->setFixedSize(60, 25);

    QLabel* ui_min = new QLabel("min", this);
    ui_min->setFont(font_s);
    ui_min->adjustSize();

    m_stopTimerBtn = new QPushButton("开启", this);
    m_stopTimerBtn->setFixedSize(60, 25);
    m_stopTimerBtn->setFont(font_s);
    connect(m_stopTimerBtn, &QPushButton::clicked, this, &Expand::slot_StopTimerBtn);

    //倒计时定时器
    m_countDownLabel = new QLabel(this);
    m_countDownLabel->setFixedSize(135, 25);
    m_countDownLabel->setFont(font_s);

    m_countDownTimer = new QTimer(this);
    connect(m_countDownTimer,&QTimer::timeout,this,[=]()mutable{
        if(m_secTime >=0 )
        {
            m_secTime--;
            m_countDownLabel->setText(QDateTime::fromSecsSinceEpoch(m_secTime, Qt::UTC).toString("hh:mm:ss"));
        }
    });

    //发送文件
    m_fileEdit = new QLineEdit(this);
    m_fileEdit->setFixedSize(180, 25);
    m_fileEdit->setFont(font_s);

    QPushButton* fileSelectBtn = new QPushButton("选择文件", this);
    fileSelectBtn->setFixedSize(60, 25);
    fileSelectBtn->setFont(font_s);
    connect(fileSelectBtn, &QPushButton::clicked, this, &Expand::slot_FileSelectBtn);

    QPushButton* fileReadBtn = new QPushButton("读取", this);
    fileReadBtn->setFixedSize(60, 25);
    fileReadBtn->setFont(font_s);
    connect(fileReadBtn, &QPushButton::clicked, this, &Expand::slot_FileReadBtn);

    //替换按钮
    QPushButton* replaceBufferBtn = new QPushButton("替换", this);
    replaceBufferBtn->setFixedSize(60, 22);
    replaceBufferBtn->setFont(font_s);
    connect(replaceBufferBtn, &QPushButton::clicked, this, &Expand::slot_ReplaceBufferBtn);

    //选项栏
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setFont(font_m);
    m_tabWidget->addTab(new QWidget(this), "编辑区1");
    m_tabWidget->addTab(new QWidget(this), "编辑区2");
    m_tabWidget->addTab(new QWidget(this), "编辑区3");
    m_tabWidget->addTab(new QWidget(this), "");
    m_tabWidget->setObjectName("tabWidget");
    m_tabWidget->setStyleSheet("QTabBar::tab { width: 75px; height: 30px;background-color:transparent;color:rgb(120,120,120);}"
                       "QTabBar::tab:selected{ color:rgb(75,75,110); border-bottom:2px solid#4b4b6e; }"
                       "QTabBar::tab:hover{ color:rgb(0,0,0); }"
                               "QTabWidget:pane{border:0px; background-color:transparent}");

    for(int i = 0; i < 3; i++)
    {
        m_textEdit[i] = new QTextEdit(m_tabWidget->widget(i));
        m_textEdit[i]->setFixedSize(340, 365);
        m_textEdit[i]->setFont(font_m);

    }

    //替换按钮
    QObject::connect(replaceBufferBtn, &QPushButton::clicked, this, &Expand::slot_ReplaceBufferBtn);
    m_tabWidget->tabBar()->setTabButton(3, QTabBar::LeftSide, replaceBufferBtn);   //替换标签
    m_tabWidget->setTabEnabled(3, false);     //禁用切换


    //配置文件
    m_textEdit[0]->setText(settings.value("编辑区1").toString());
    m_textEdit[1]->setText(settings.value("编辑区2").toString());
    m_textEdit[2]->setText(settings.value("编辑区3").toString());
    m_timeEdit->setText(settings.value("定时器").toString());
    connect(m_textEdit[0], &QTextEdit::textChanged, this, [&]{settings.setValue("编辑区1", m_textEdit[0]->toPlainText());});
    connect(m_textEdit[1], &QTextEdit::textChanged, this, [&]{settings.setValue("编辑区2", m_textEdit[1]->toPlainText());});
    connect(m_textEdit[2], &QTextEdit::textChanged, this, [&]{settings.setValue("编辑区3", m_textEdit[2]->toPlainText());});
    connect(m_timeEdit, &QLineEdit::textChanged, this, [&]{settings.setValue("定时器", m_timeEdit->text());});


    //布局管理器
    QHBoxLayout* hBox1 = new QHBoxLayout;
    hBox1->addWidget(ui_timer);
    hBox1->addWidget(m_timeEdit);
    hBox1->addWidget(ui_min);
    hBox1->addWidget(m_stopTimerBtn);
    hBox1->addWidget(m_countDownLabel);

    QHBoxLayout* hBox2 = new QHBoxLayout;
    hBox2->addWidget(m_fileEdit);
    hBox2->addWidget(fileSelectBtn);
    hBox2->addWidget(fileReadBtn);

    QVBoxLayout* vBox = new QVBoxLayout;
    vBox->addLayout(hBox1);
    vBox->addLayout(hBox2);
    vBox->addWidget(m_tabWidget);
    vBox->setSpacing(15);

    this->setLayout(vBox);
}


