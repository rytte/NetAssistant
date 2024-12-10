#include "AgentTest.h"
#pragma execution_character_set("utf-8")
#include <QJsonObject>
#include <QLabel>
#include <QStringList>
#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QMessageBox>
#include "mainwindow.h"
#include <QRandomGenerator>


AgentTest::AgentTest(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(0, 15, 450, 650);
    m_myParent = (MainWindow* )topLevelWidget();

    //创建UI界面
    uiBuild();

    //更新布局
    slot_CommandBoxChanged();
    slot_LimitStyleBoxChanged();
}

void AgentTest::slot_BuildBtnSZ()
{
    QJsonObject root;
    QJsonObject body;
    QJsonObject header;
    QJsonObject messageBody;
    messageBody.insert("stationsvr_id", "PC-Bk_Test");
    messageBody.insert("stationsvr_user", "root");
    messageBody.insert("stationsvr_pass", "admin");
    messageBody.insert("devid", "0.22.1.8.1");
    messageBody.insert("user", "central");
    messageBody.insert("pass", "central");

    QString strCommand = m_commandBox->currentText();

    //数据设置
    if("PLAY_EXCLUSIVE_TEXT" == strCommand || "PLAY_PROMPT_TEXT" == strCommand || "DEFAULT_PROMPT_TEXT" == strCommand)
    {

        //文本滚动速度设置
        int iScrollInterval = -1;
        QString strScrollSpeed = m_textScrollIntervalBox->currentText();
        if("Min_Low" == strScrollSpeed)
            iScrollInterval = INTERVAL_1;
        else if("Low" == strScrollSpeed)
            iScrollInterval = INTERVAL_2;
        else if("Fast" == strScrollSpeed)
            iScrollInterval = INTERVAL_3;
        else if("Max_Fast" == strScrollSpeed)
            iScrollInterval = INTERVAL_4;

        //文本字体大小
        int iTextFontSize = -1;
        QString strFontSize = m_textFontSizeEdit->text();
        if("" != strFontSize && strFontSize.toInt() > 0)
            iTextFontSize = strFontSize.toInt();

        messageBody.insert("msg_priority", "s");                                   //紧急文本或提示文本默认下发等级
        messageBody.insert("sceneLayout", "d2a9e819-fc73-455b-87cc-9610f5169e8e"); //默认紧急版式ID
        messageBody.insert("text", m_textBox->currentText());
        messageBody.insert("background_color", m_backgroundColorBox->currentText());
        messageBody.insert("text_color", m_textColorBox->currentText());
        messageBody.insert("text_scrollInterval", iScrollInterval);
        messageBody.insert("text_fontSize", iTextFontSize);
        messageBody.insert("text_direction", m_textDirectionBox->currentText());

        //是否限时判断(0-不限时;其他值则代表显示相应的时长(min))
        QString strLimitType = m_limitStyleBox->currentText();
        if("unlimited" == strLimitType)
            messageBody.insert("limit_style", "unlimited");
        else if("limit_time" == strLimitType)
        {
            messageBody.insert("limit_style", "limit_time");
            messageBody.insert("limit_time", m_limitTimeBox->text());
        }
        else if("timer" == strLimitType)
        {
            messageBody.insert("limit_style", "timer");
            QString strStartTime = m_limitTimeStartBox->dateTime().toString("yyyy/MM/dd hh:mm:ss");
            QString strEndTime = m_limitTimeEndBox->dateTime().toString("yyyy/MM/dd hh:mm:ss");

            if(m_limitTimeEndBox->dateTime() > QDateTime::currentDateTime())
                messageBody.insert("limit_time", strStartTime+","+strEndTime);
            else
            {
                QMessageBox::warning(this, "Warning", "时间设置异常！结束时间小于当前时间，显示未更新！");
                return;
            }
        }

    }
    else if("SET_VOLUME" == strCommand)
    {
        messageBody.insert("volume", m_volumeBox->text());
    }

    //字符串形式添加
    QJsonDocument messageBodyJsonDoc(messageBody);
    QString str = messageBodyJsonDoc.toJson(QJsonDocument::Compact);
    body.insert("messageBody", str);

    body.insert("messageCommand", strCommand);
    root.insert("body", body);

    //头部分
    header.insert("end", true);
    header.insert("messageNo", "39c43a88-ec11-4e02-9400-e2fd7969bb35");
    header.insert("messageType", "P2P_command_message");
    header.insert("messageReceiver", "bk_player");
    header.insert("messageSender", "Bk_Test_Tool");
    header.insert("timeout", 1);
    header.insert("zzflag", "end");
    root.insert("header", header);

    //生成
    QJsonDocument rootJsonDoc(root);
    m_myParent->setSendBuffer(rootJsonDoc.toJson(QJsonDocument::Compact));
}

void AgentTest::slot_BuildBtnFZ()
{
    QJsonObject root;
    QJsonObject messageBody;

    QString strCommand = m_commandBox->currentText();


    //数据设置
    if("PLAY_EXCLUSIVE_TEXT" == strCommand || "PLAY_PROMPT_TEXT" == strCommand || "DEFAULT_PROMPT_TEXT" == strCommand)
    {
        //文本滚动速度设置
        int iScrollInterval = -1;
        QString strScrollSpeed = m_textScrollIntervalBox->currentText();
        if("Min_Low" == strScrollSpeed)
            iScrollInterval = INTERVAL_1;
        else if("Low" == strScrollSpeed)
            iScrollInterval = INTERVAL_2;
        else if("Fast" == strScrollSpeed)
            iScrollInterval = INTERVAL_3;
        else if("Max_Fast" == strScrollSpeed)
            iScrollInterval = INTERVAL_4;

        //文本字体大小
        int iTextFontSize = -1;
        QString strFontSize = m_textFontSizeEdit->text();
        if("" != strFontSize && strFontSize.toInt() > 0)
            iTextFontSize = strFontSize.toInt();

        //添加键值对
        messageBody.insert("msg_priority", m_msgPriorityBox->text().toInt());
        messageBody.insert("text", m_textBox->currentText());
        messageBody.insert("background_color", m_backgroundColorBox->currentText());
        messageBody.insert("text_color", m_textColorBox->currentText());
        messageBody.insert("text_scrollInterval", iScrollInterval);
        messageBody.insert("text_fontSize", iTextFontSize);
        messageBody.insert("text_direction", m_textDirectionBox->currentText());


        //是否限时判断(0-不限时;其他值则代表显示相应的时长(min))
        QString strLimitType = m_limitStyleBox->currentText();
        if("unlimited" == strLimitType)
            messageBody.insert("limit_style", "unlimited");
        else if("limit_time" == strLimitType)
        {
            messageBody.insert("limit_style", "limit_time");
            messageBody.insert("limit_time", m_limitTimeBox->text());
        }
        else if("timer" == strLimitType)
        {
            messageBody.insert("limit_style", "timer");
            QString strStartTime = m_limitTimeStartBox->dateTime().toString("yyyy-MM-dd hh:mm:ss");
            QString strEndTime = m_limitTimeEndBox->dateTime().toString("yyyy-MM-dd hh:mm:ss");

            if(m_limitTimeEndBox->dateTime() > QDateTime::currentDateTime())
                messageBody.insert("limit_time", strStartTime+","+strEndTime);
            else
            {
                QMessageBox::warning(this, "Warning", "时间设置异常！结束时间小于当前时间，显示未更新！");
                return;
            }
        }
    }
    else if("SET_VOLUME" == strCommand)
    {
        messageBody.insert("volume", m_volumeBox->text());
    }

    int randomInt = QRandomGenerator::global()->bounded(900000000) + 100000000;
    messageBody.insert("text_id", QString::number(randomInt));
    randomInt = QRandomGenerator::global()->bounded(900000000) + 100000000;
    root.insert("messageNo", QString::number(randomInt));
    root.insert("messageBody", messageBody);
    root.insert("messageCommand", strCommand);

    //生成
    QJsonDocument rootJsonDoc(root);
    m_myParent->setSendBuffer(rootJsonDoc.toJson(QJsonDocument::Indented));
}

void AgentTest::slot_ConvertJsonBtnSZ()
{
    QString str = m_myParent->getSendBuffer();

    QString convertStr = "";
    QJsonParseError err;
    QJsonDocument rootDoc = QJsonDocument::fromJson(str.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError)		//判断数据是否符合json格式
    {
        QMessageBox::information(this, "注意", "Json格式错误");
        return;
    }

    QJsonObject root = rootDoc.object();
    QJsonObject body = root.value("body").toObject();

    //不含body表示福州的Json，直接退出
    if(!root.contains("body"))
        return;

    //检查格式，含'\n'表示当前为缩进形式
    if(str.contains('\n'))
    {
        //获取messageBody的Json对象并转换为字符串
        QJsonObject messageBody = body.value("messageBody").toObject();
        QJsonDocument messageBodyJsonDoc(messageBody);
        QString strMessageBody = messageBodyJsonDoc.toJson(QJsonDocument::Compact);
        //移除原先键值对，插入新键值对
        body.remove("messageBody");
        body.insert("messageBody", strMessageBody);
        root.remove("body");
        root.insert("body", body);

        QJsonDocument jsonDoc(root);
        convertStr = jsonDoc.toJson(QJsonDocument::Compact);
    }
    else    //紧凑形式
    {
        //字符串类型转换为Json对象
        QString strMessageBody = body.value("messageBody").toString();
        QJsonDocument doc = QJsonDocument::fromJson(strMessageBody.toUtf8(), &err);
        if (err.error != QJsonParseError::NoError)		//判断数据是否符合json格式
        {
            QMessageBox::information(this, "注意", "messageBody的Json格式错误");
            return;
        }

        QJsonObject messageBody = doc.object();
        body.remove("messageBody");
        body.insert("messageBody", messageBody);
        root.remove("body");
        root.insert("body", body);

        QJsonDocument jsonDoc(root);
        convertStr = jsonDoc.toJson(QJsonDocument::Indented);
    }

    m_myParent->setSendBuffer(convertStr);
}

//控件隐藏与显示
void AgentTest::slot_CommandBoxChanged()
{
    QString strCommand = m_commandBox->currentText();

    if("PLAY_EXCLUSIVE_TEXT" == strCommand || "PLAY_PROMPT_TEXT" == strCommand || "DEFAULT_PROMPT_TEXT" == strCommand)
    {
        ui_label[12]->hide();
        m_widgetVec[12]->hide();
        for(int i = 0; i < 12; i++)
        {
            ui_label[i]->show();
            m_widgetVec[i]->show();
        }
    }
    else if("SET_VOLUME" == strCommand)
    {
        ui_label[12]->show();
        m_widgetVec[12]->show();
        for(int i = 1; i < 12; i++)
        {
            ui_label[i]->hide();
            m_widgetVec[i]->hide();
        }
    }
    else
    {
        for(int i = 1; i < 13; i++)
        {
            ui_label[i]->hide();
            m_widgetVec[i]->hide();
        }
    }

}

//控件隐藏与显示
void AgentTest::slot_LimitStyleBoxChanged()
{
    QString strLimitStyle = m_limitStyleBox->currentText();

    if("unlimited" == strLimitStyle)
    {
        m_limitTimeBox->setEnabled(false);
        m_limitTimeStartBox->setEnabled(false);
        m_limitTimeEndBox->setEnabled(false);
    }
    else if("limit_time" == strLimitStyle)
    {
        m_limitTimeBox->setEnabled(true);
        m_limitTimeStartBox->setEnabled(false);
        m_limitTimeEndBox->setEnabled(false);
    }
    else if("timer" == strLimitStyle)
    {
        m_limitTimeBox->setEnabled(false);
        m_limitTimeStartBox->setEnabled(true);
        m_limitTimeEndBox->setEnabled(true);
    }
}

void AgentTest::uiBuild()
{
    //命令项
    m_commandBox = new QComboBox(this);
    m_commandBox->addItem("DEFAULT_PROMPT_TEXT");
    m_commandBox->addItem("PLAY_PROMPT_TEXT");
    m_commandBox->addItem("PLAY_EXCLUSIVE_TEXT");
    m_commandBox->addItem("READ_ARRANGEMENT");
    m_commandBox->addItem("RECOVER_TO_PLAN_MODE");
    m_commandBox->addItem("RECOVER_EXCLUSIVE_TEXT");
    m_commandBox->addItem("RECOVER_PROMPT_TEXT");
    m_commandBox->addItem("RECOVER_EMERGENCY_TEXT");
    m_commandBox->addItem("SET_VOLUME");
    m_commandBox->addItem("PROCESS_EXIT");
    connect(m_commandBox, &QComboBox::currentTextChanged, this, &AgentTest::slot_CommandBoxChanged);

    //文本内容
    m_textBox = new QComboBox(this);
    m_textBox->addItem("各位乘客请注意，由于深圳突降暴雨，地面湿滑，请各位乘客有序排队进站乘车，切勿拥挤，谢谢配合！");
    m_textBox->addItem("Dear passengers, please note that due to the sudden rainstorm in Shenzhen, the ground is wet and slippery. Please line up in order to get on the bus. Do not crowd. Thank you for your cooperation!");
    m_textBox->addItem("秋天是丰收的季节，田野里金黄一片，稻谷弯下了腰，沉甸甸的果实挂满了枝头。");
    m_textBox->addItem("冬天悄然而至，寒冷的空气让人不禁打了个寒颤。大地被白雪覆盖，世界仿佛变成了一个银色的童话。空气清冷，呼出的气息在空中凝结成白雾。树木光秃，枝条上偶尔积雪，形成了美丽的冰雕。夜晚，家家户户的窗户透出温暖的灯光，炉火旁，人们围坐在一起，享受着温暖和亲情。");

    //背景颜色
    m_backgroundColorBox = new QComboBox(this);
    m_backgroundColorBox->addItem("white");
    m_backgroundColorBox->addItem("black");
    m_backgroundColorBox->addItem("red");
    m_backgroundColorBox->addItem("blue");

    //文本颜色
    m_textColorBox = new QComboBox(this);
    m_textColorBox->addItem("black");
    m_textColorBox->addItem("white");
    m_textColorBox->addItem("red");
    m_textColorBox->addItem("blue");

    //滚动方向
    m_textDirectionBox = new QComboBox(this);
    m_textDirectionBox->addItem("to_left");
    m_textDirectionBox->addItem("to_right");
    m_textDirectionBox->addItem("to_up");
    m_textDirectionBox->addItem("static");

    //滚动速度
    m_textScrollIntervalBox = new QComboBox(this);
    m_textScrollIntervalBox->addItem("");
    m_textScrollIntervalBox->addItem("Min_Low");
    m_textScrollIntervalBox->addItem("Low");
    m_textScrollIntervalBox->addItem("Fast");
    m_textScrollIntervalBox->addItem("Max_Fast");

    //字体大小
    m_textFontSizeEdit = new QLineEdit(this);

    //时间限制
    m_limitStyleBox = new QComboBox(this);
    m_limitStyleBox->addItem("unlimited");
    m_limitStyleBox->addItem("limit_time");
    m_limitStyleBox->addItem("timer");
    connect(m_limitStyleBox, &QComboBox::currentTextChanged, this, &AgentTest::slot_LimitStyleBoxChanged);

    //显示时长
    m_limitTimeBox = new QSpinBox(this);

    //开始时间
    m_limitTimeStartBox = new QDateTimeEdit(this);
    m_limitTimeStartBox->setDate(QDate(2024, 7, 1));
    m_limitTimeStartBox->setTime(QTime(8, 0));

    //结束时间
    m_limitTimeEndBox = new QDateTimeEdit(this);
    m_limitTimeEndBox->setDate(QDate(2024, 7, 1));
    m_limitTimeEndBox->setTime(QTime(8, 0));

    //音量
    m_volumeBox = new QSpinBox(this);
    m_volumeBox->setMaximum(100);
    m_volumeBox->setMinimum(0);

    //消息优先级
    m_msgPriorityBox = new QSpinBox(this);
    m_msgPriorityBox->setMaximum(10);
    m_msgPriorityBox->setMinimum(1);
    m_msgPriorityBox->setValue(6);

    //生成按钮SZ
    m_buildBtnSZ = new QPushButton(this);
    m_buildBtnSZ->setText("生成深圳JSON");
    m_buildBtnSZ->setFont(font_m);
    connect(m_buildBtnSZ, &QPushButton::clicked, this, &AgentTest::slot_BuildBtnSZ);

    //生成按钮FZ
    m_buildBtnFZ = new QPushButton(this);
    m_buildBtnFZ->setText("生成福州JSON");
    m_buildBtnFZ->setFont(font_m);
    connect(m_buildBtnFZ, &QPushButton::clicked, this, &AgentTest::slot_BuildBtnFZ);

    //深圳Json转换按钮
    m_jsonConvertSZ = new QPushButton(this);
    m_jsonConvertSZ->setText("转换");
    m_jsonConvertSZ->setFont(font_s);
    m_jsonConvertSZ->setMaximumSize(70, 40);
    connect(m_jsonConvertSZ, &QPushButton::clicked, this, &AgentTest::slot_ConvertJsonBtnSZ);

    //UDP发送按钮
    m_udpSendBtn = new QPushButton(this);
    m_udpSendBtn->setText("UDP发送");
    m_udpSendBtn->setFont(font_s);
    m_udpSendBtn->setMaximumSize(70, 40);
    connect(m_udpSendBtn, &QPushButton::clicked, m_myParent, &MainWindow::slot_UdpSendBtn);


    //管理
    m_widgetVec.push_back(m_commandBox);
    m_widgetVec.push_back(m_textBox);
    m_widgetVec.push_back(m_backgroundColorBox);
    m_widgetVec.push_back(m_textColorBox);
    m_widgetVec.push_back(m_textDirectionBox);
    m_widgetVec.push_back(m_textScrollIntervalBox);
    m_widgetVec.push_back(m_textFontSizeEdit);
    m_widgetVec.push_back(m_limitStyleBox);
    m_widgetVec.push_back(m_limitTimeBox);
    m_widgetVec.push_back(m_limitTimeStartBox);
    m_widgetVec.push_back(m_limitTimeEndBox);
    m_widgetVec.push_back(m_msgPriorityBox);
    m_widgetVec.push_back(m_volumeBox);


    //UI文本
    QString ui_labelText[13] = {"命令项", "文本内容", "背景颜色", "文本颜色", "滚动方向", "滚动速度",
                                "字体大小", "时间限制", "显示时长", "开始时间", "结束时间", "优先级", "设置音量"};

    for(int i = 0; i < 13; i++)
    {
        ui_label[i] = new QLabel(this);
        ui_label[i]->setAlignment(Qt::AlignCenter);
        ui_label[i]->setText(ui_labelText[i]);

        //布局设置
        ui_label[i]->setFixedSize(80, 30);
        m_widgetVec[i]->setFixedSize(300, 30);
    }


    //水平布局
    for(int i = 0; i < 13; i++)
    {
        m_hBox[i] = new QHBoxLayout;
        m_hBox[i]->addWidget(ui_label[i]);
        m_hBox[i]->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding));
        m_hBox[i]->addWidget(m_widgetVec[i]);
    }

    m_hBox[13] = new QHBoxLayout;
    m_hBox[13]->addWidget(m_jsonConvertSZ);
    m_hBox[13]->addSpacing(10);
    m_hBox[13]->addWidget(m_buildBtnSZ);
    m_hBox[13]->addSpacing(10);
    m_hBox[13]->addWidget(m_buildBtnFZ);
    m_hBox[13]->addSpacing(10);
    m_hBox[13]->addWidget(m_udpSendBtn);

    //垂直布局
    m_vBox = new QVBoxLayout;
    for(int i = 0; i < 14; i++)
    {
        m_vBox->addLayout(m_hBox[i]);
        m_vBox->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding));
    }
    setLayout(m_vBox);
}











