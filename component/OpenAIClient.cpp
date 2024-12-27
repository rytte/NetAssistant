#include "OpenAIClient.h"
#include <QApplication>
#include <QMessageBox>
#include "mainwindow.h"


OpenAIClient::OpenAIClient(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(0, 15, 340, 495);
    setStyleSheet("QWidget { font-family: 'Microsoft YaHei'; font-size: 12px; }");

    //获取父控件对象
    m_myParent = (MainWindow* )topLevelWidget();

    //创建UI界面
    uiBuild();

    m_netManager = new QNetworkAccessManager(this);
    m_messages.append(QJsonObject({{"role", "developer"}, {"content", "你是一个诚实的AI助手，会尽自己所能帮助别人，实事求是地回答问题"}}));

    //获取模型
    slot_getBtn();
}

void OpenAIClient::slot_getBtn()
{
    // 设置请求头
    QUrl url(m_apiManager.getModelUrl(m_serviceBox->currentText()));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + m_apiKeyEdit->text().toUtf8());

    // 发送 Get 请求
    QNetworkReply *reply = m_netManager->get(request);

    // 连接信号，接收响应
    connect(reply, &QNetworkReply::finished, this, &OpenAIClient::slot_responseGet);
}

void OpenAIClient::slot_postBtn()
{
    if(m_modelBox->currentText().isEmpty())
    {
        QMessageBox::warning(this, "未选择模型", "请先选择模型");
        return;
    }

    QString ask = m_myParent->getSendBuffer();

//    m_textBrowser->append("<p style='font-family: \"Microsoft YaHei\", sans-serif; font-size: 16px; font-weight: bold; line-height: 0.5;'>User</p>");
    m_textBrowser->append("【User】");
    m_textBrowser->append(ask);
    m_textBrowser->append("\n");

    // 限制最大上下文长度：10条
    while(m_messages.size() >= 10)
        m_messages.removeAt(1);

    qDebug() << "Current context size: " << m_messages.size();

    // 构造 JSON 请求体
    QJsonObject requestBody;
    requestBody["model"] = m_modelBox->currentText();
    m_messages.append(QJsonObject({{"role", "user"}, {"content", ask}}));
    requestBody["messages"] = m_messages;
    requestBody["stream"] = false;
//        requestBody["max_tokens"] = 100;
//        requestBody["temperature"] = 0.5;
//        requestBody["top_p"] = 1.0;

    QJsonDocument doc(requestBody);
    QByteArray requestData = doc.toJson();

    // 设置请求头
    QUrl url(m_apiManager.getChatUrl(m_serviceBox->currentText()));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + m_apiKeyEdit->text().toUtf8());

    // 发送 POST 请求
    QNetworkReply *reply = m_netManager->post(request, requestData);

    // 连接信号，接收响应
    connect(reply, &QNetworkReply::finished, this, &OpenAIClient::slot_responsePost);
}

void OpenAIClient::slot_responseGet()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    QJsonObject responseObject = checkReply(reply);
    if(responseObject.isEmpty())
        return;

    // 提取生成的文本
    if (responseObject.contains("data") && responseObject["data"].isArray())  ///OpenAI响应
    {
        QJsonArray dataArr = responseObject["data"].toArray();
        m_modelBox->clear();
        for(int i = 0; i < dataArr.size(); i++)
        {
            QJsonObject data = dataArr[i].toObject();
            if (data.contains("id"))
            {
                QString modelID = data["id"].toString();
                m_modelBox->addItem(modelID);
            }
        }
    }
    else        ///Ollama响应
    {
        if (responseObject.contains("models") && responseObject["models"].isArray())
        {
            QJsonArray modelArr = responseObject["models"].toArray();
            m_modelBox->clear();
            for(int i = 0; i < modelArr.size(); i++)
            {
                QJsonObject model = modelArr[i].toObject();
                if (model.contains("name"))
                {
                    QString name = model["name"].toString();
                    m_modelBox->addItem(name);
                }
            }
        }
    }

    // 初始化时自动选择模型
    static bool init = true;
    if(init)
    {
        m_modelBox->setCurrentText(m_model);
        connect(m_modelBox, &QComboBox::currentTextChanged, this, [&]{
            settings.setValue("Model", m_modelBox->currentText());
            m_model = m_modelBox->currentText();
        });
        init = false;
    }

    // 清理
    reply->deleteLater();
}

void OpenAIClient::slot_responsePost()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    QJsonObject responseObject = checkReply(reply);
    if(responseObject.isEmpty())
        return;

//    m_textBrowser->append("<p style='font-family: \"Microsoft YaHei\", sans-serif; font-size: 16px; font-weight: bold;'>Assistant</p>");
    m_textBrowser->append("【Assistant】");

    // 提取生成的文本
    if (responseObject.contains("choices") && responseObject["choices"].isArray())  ///OpenAI响应
    {
        QJsonArray choices = responseObject["choices"].toArray();
        if (!choices.isEmpty())
        {
            QJsonObject choice = choices[0].toObject();
            if (choice.contains("message"))
            {
                QString generatedText = choice["message"].toObject()["content"].toString();
                m_messages.append(QJsonObject({{"role", "assistant"}, {"content", generatedText}}));
                m_textBrowser->append(generatedText);
                qDebug() << "Generated text:" << generatedText;
            }
        }
    }
    else        ///Ollama响应
    {
        if (responseObject.contains("message"))
        {
            QString generatedText = responseObject["message"].toObject()["content"].toString();
            m_messages.append(QJsonObject({{"role", "assistant"}, {"content", generatedText}}));
            m_textBrowser->append(generatedText);
            qDebug() << "Generated text:" << generatedText;
        }
    }
    m_textBrowser->append("\n");

    // 清理
    reply->deleteLater();
}

QJsonObject OpenAIClient::checkReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        m_textBrowser->append("Request failed:" + reply->errorString());
        qWarning() << "Request failed:" << reply->errorString();
        return {};
    }

    // 获取响应数据
    QByteArray responseData = reply->readAll();

    // 解析 JSON 响应
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if (!jsonDoc.isObject())
    {
        m_textBrowser->append("Invalid JSON response");
        qWarning() << "Invalid JSON response";
        return {};
    }

    return jsonDoc.object();
}

void OpenAIClient::uiBuild()
{
    // 平台
    QLabel* service = new QLabel("Service", this);
    service->setFixedSize(50, 25);

    m_serviceBox = new QComboBox(this);
    m_serviceBox->setFixedSize(249, 25);
    m_serviceBox->addItems(m_apiManager.getServiceNames());
    connect(m_serviceBox, &QComboBox::currentTextChanged, this, [this]{
        if(m_serviceBox->currentText().contains("ollama"))
            m_apiKeyEdit->setEnabled(false);
        else
            m_apiKeyEdit->setEnabled(true);
    });

    // 密钥
    QLabel* apiKey = new QLabel("API Key", this);
    apiKey->setFixedSize(50, 25);

    m_apiKeyEdit = new QLineEdit(this);
    m_apiKeyEdit->setFixedSize(213, 25);
    m_apiKeyEdit->setEchoMode(QLineEdit::Password);
    if(m_serviceBox->currentText().contains("ollama"))
        m_apiKeyEdit->setEnabled(false);
    else
        m_apiKeyEdit->setEnabled(true);

    // 创建显示/隐藏密码按钮
    QPushButton* toggleButton = new QPushButton(this);
    toggleButton->setFixedSize(20, 20);
    QIcon toggleIcon = QApplication::style()->standardIcon(QStyle::SP_BrowserReload);
    toggleButton->setIcon(toggleIcon);
    connect(toggleButton, &QPushButton::clicked, this, [this]{
        if (m_apiKeyEdit->echoMode() == QLineEdit::Password)
            m_apiKeyEdit->setEchoMode(QLineEdit::Normal);
        else
            m_apiKeyEdit->setEchoMode(QLineEdit::Password);
    });

    // 模型
    QLabel* model = new QLabel("Model", this);
    model->setFixedSize(50, 25);

    m_modelBox = new QComboBox(this);
    m_modelBox->setFixedSize(110, 25);

    // 拉取模型
    QPushButton* getBtn = new QPushButton("Get", this);
    getBtn->setFixedSize(45, 25);
    connect(getBtn, &QPushButton::clicked, this, &OpenAIClient::slot_getBtn);

    // 发送消息
    QPushButton* postBtn = new QPushButton("Post", this);
    postBtn->setFixedSize(65, 25);
    connect(postBtn, &QPushButton::clicked, this, &OpenAIClient::slot_postBtn);

    // 文本显示
    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setFixedSize(315, 368);
    m_textBrowser->document()->setMaximumBlockCount(2000);


    // 配置文件
    m_serviceBox->setCurrentText(settings.value("Service").toString());
    m_apiKeyEdit->setText(settings.value("API Key").toString());
    m_model = settings.value("Model").toString();
    connect(m_serviceBox, &QComboBox::currentTextChanged, this, [&]{settings.setValue("Service", m_serviceBox->currentText());});
    connect(m_apiKeyEdit, &QLineEdit::textChanged, this, [&]{settings.setValue("API Key", m_apiKeyEdit->text());});


    // 布局管理器
    QHBoxLayout* hbox[3];
    hbox[0] = new QHBoxLayout;
    hbox[0]->addWidget(service);
    hbox[0]->addWidget(m_serviceBox);
    hbox[1] = new QHBoxLayout;
    hbox[1]->addWidget(apiKey);
    hbox[1]->addWidget(m_apiKeyEdit);
    hbox[1]->addWidget(toggleButton);
    hbox[2] = new QHBoxLayout;
    hbox[2]->addWidget(model);
    hbox[2]->addWidget(m_modelBox);
    hbox[2]->addWidget(getBtn);
    hbox[2]->addWidget(postBtn);

    QVBoxLayout* vBox = new QVBoxLayout;
    vBox->addLayout(hbox[0]);
    vBox->addLayout(hbox[1]);
    vBox->addLayout(hbox[2]);
    vBox->addWidget(m_textBrowser);
    vBox->setSpacing(10);

    this->setLayout(vBox);
}
