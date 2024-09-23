#include "Expand.h"
#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include <QDateTime>


Expand::Expand(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(0, 15, 450, 650);

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
    m_FileEdit->setText(filePath);
}

void Expand::slot_FileReadBtn()
{
    QFile readFile(m_FileEdit->text());
    readFile.open(QIODevice::ReadOnly);
    m_myParent->setSendBuffer(QString::fromUtf8(readFile.readAll()));
    readFile.close();
}

void Expand::uiBuild()
{
    //定时器
    m_stopTimer = new QTimer(this);
    connect(m_stopTimer, &QTimer::timeout, m_myParent, &MainWindow::slot_StopKeepBtn);

    QLabel* ui_timer = new QLabel(this);
    ui_timer->setGeometry(10, 25, 0, 0);
    ui_timer->setText("定时器：");
    ui_timer->setFont(font_m);
    ui_timer->adjustSize();

    m_timeEdit = new QLineEdit(this);
    m_timeEdit->setGeometry(85, 20, 80, 30);

    QLabel* ui_min = new QLabel(this);
    ui_min->setGeometry(175, 25, 0, 0);
    ui_min->setText("min");
    ui_min->setFont(font_s);
    ui_min->adjustSize();

    m_stopTimerBtn = new QPushButton(this);
    m_stopTimerBtn->setGeometry(235, 20, 80, 30);
    m_stopTimerBtn->setFont(font_s);
    m_stopTimerBtn->setText("开启");
    connect(m_stopTimerBtn, &QPushButton::clicked, this, &Expand::slot_StopTimerBtn);

    //倒计时定时器
    m_countDownLabel = new QLabel(this);
    m_countDownLabel->setGeometry(360, 20, 180, 30);
    m_countDownLabel->setFont(font_s);

    m_countDownTimer = new QTimer(this);
    connect(m_countDownTimer,&QTimer::timeout,this,[=]()mutable{
        if(m_secTime>=0)
        {
            m_secTime--;
            m_countDownLabel->setText(QDateTime::fromSecsSinceEpoch(m_secTime, Qt::UTC).toString("hh:mm:ss"));
        }
    });

    //发送文件
    m_FileEdit = new QLineEdit(this);
    m_FileEdit->setGeometry(0, 70, 250, 30);
    m_FileEdit->setFont(font_s);

    m_FileSelectBtn = new QPushButton(this);
    m_FileSelectBtn->setGeometry(280, 70, 80, 30);
    m_FileSelectBtn->setFont(font_s);
    m_FileSelectBtn->setText("选择文件");
    connect(m_FileSelectBtn, &QPushButton::clicked, this, &Expand::slot_FileSelectBtn);

    m_FileReadBtn = new QPushButton(this);
    m_FileReadBtn->setGeometry(370, 70, 80, 30);
    m_FileReadBtn->setFont(font_s);
    m_FileReadBtn->setText("读取");
    connect(m_FileReadBtn, &QPushButton::clicked, this, &Expand::slot_FileReadBtn);

    //选项栏
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setGeometry(0, 120, 450, 620);
    QWidget* buffer = new QWidget(this);
    QWidget* buffer2 = new QWidget(this);
    QWidget* buffer3 = new QWidget(this);
    buffer->resize(150, 35);
    buffer2->resize(150, 35);
    buffer3->resize(150, 35);
    m_tabWidget->setFont(font_m);
    m_tabWidget->addTab(buffer, "编辑区1");
    m_tabWidget->addTab(buffer2, "编辑区2");
    m_tabWidget->addTab(buffer3, "编辑区3");
    m_tabWidget->setObjectName("tabWidget");
    m_tabWidget->setStyleSheet("QTabBar::tab { width: 100px; height: 35px;background-color:transparent;color:rgb(120,120,120);}"
                       "QTabBar::tab:selected{ color:rgb(75,75,110); border-bottom:2px solid#4b4b6e; }"
                       "QTabBar::tab:hover{ color:rgb(0,0,0); }"
                               "QTabWidget:pane{border:0px; background-color:transparent}");

    for(int i = 0; i < 3; i++)
    {
        m_textEdit[i] = new QTextEdit(m_tabWidget->widget(i));
        m_textEdit[i]->setGeometry(0, 10, 450, 480);
        m_textEdit[i]->setFont(font_m);

    }

    //替换按钮
    m_replaceBufferBtn = new QPushButton(this);
    m_replaceBufferBtn->setGeometry(350, 120, 100, 35);
    m_replaceBufferBtn->setText("替换");
    m_replaceBufferBtn->setFont(font_s);
    connect(m_replaceBufferBtn, &QPushButton::clicked, this, &Expand::slot_ReplaceBufferBtn);
}


