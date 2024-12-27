#ifndef OPENAICLIENT_H
#define OPENAICLIENT_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QDebug>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QStyle>
#include <QComboBox>
#include <QString>
#include <QMap>
#include <QSettings>
#include <QHBoxLayout>
#include <QVBoxLayout>

class MainWindow;

class APIManager
{
public:
    struct Service
    {
        QString chatUrl;
        QString modelUrl;
    };

    // 构造函数，初始化服务地址
    APIManager()
    {
        services_["ollama"] = {"http://127.0.0.1:11434/api/chat", "http://127.0.0.1:11434/api/tags"};                       //ollama: 无需APIKey
        services_["openai"] = {"https://api.openai.com/v1/chat/completions", "https://api.openai.com/v1/models"};           //openai官方
        services_["xiaoai"] = {"https://api.xiaoai.plus/v1/chat/completions", "https://api.xiaoai.plus/v1/models"};         //三方中转: api通常与openai兼容
    }

    // 获取所有服务商的名称
    QStringList getServiceNames() const
    {
        return services_.keys(); // 返回所有服务商的名称列表
    }

    // 获取指定服务的 chat URL
    QString getChatUrl(const QString& service)
    {
        if (services_.contains(service))
            return services_[service].chatUrl;
        else
            return QString();
    }

    // 获取指定服务的 model URL
    QString getModelUrl(const QString& service)
    {
        if (services_.contains(service))
            return services_[service].modelUrl;
        else
            return QString();
    }

private:
    QMap<QString, Service> services_; // 用来存储服务的地址
};


class OpenAIClient : public QWidget
{
    Q_OBJECT

public:
    explicit OpenAIClient(QWidget *parent = nullptr);

private slots:
    void slot_getBtn();     //拉取模型
    void slot_postBtn();    //发送消息
    //处理响应消息
    void slot_responseGet();
    void slot_responsePost();

private:
    QJsonObject checkReply(QNetworkReply *reply);
    void uiBuild();

    MainWindow* m_myParent;
    APIManager m_apiManager;
    QNetworkAccessManager *m_netManager;
    QJsonArray m_messages;
    QString m_model;

    //ui控件
    QComboBox* m_serviceBox;
    QLineEdit* m_apiKeyEdit;
    QComboBox* m_modelBox;
    QTextBrowser* m_textBrowser;
};

#endif // OPENAICLIENT_H
