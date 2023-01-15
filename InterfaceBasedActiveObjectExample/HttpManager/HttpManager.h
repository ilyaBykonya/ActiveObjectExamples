#pragma once
#include <QNetworkAccessManager>
#include <QPointer>
#include <QPromise>
#include <QFuture>
#include <QEvent>

class HttpManager : public QObject {
private:
    class HttpGetRequestEvent : public QEvent {
    private:
        QPromise<std::shared_ptr<QNetworkReply>> m_promise;
        const QNetworkRequest m_request;
    public:
        inline static constexpr QEvent::Type Type = static_cast<QEvent::Type>(QEvent::Type::User + 1);
        HttpGetRequestEvent(const QNetworkRequest &request);
    public:
        QPromise<std::shared_ptr<QNetworkReply>>& promise();
        const QNetworkRequest& request() const;
    };
    class HttpPostRequestEvent : public QEvent {
    private:
        QPromise<std::shared_ptr<QNetworkReply>> m_promise;
        const QNetworkRequest m_request;
        const QByteArray m_data;
    public:
        inline static constexpr QEvent::Type Type = static_cast<QEvent::Type>(QEvent::Type::User + 2);
        HttpPostRequestEvent(const QNetworkRequest &request, const QByteArray &data);
    public:
        QPromise<std::shared_ptr<QNetworkReply>>& promise();
        const QNetworkRequest& request() const;
        const QByteArray& data() const;
    };
private:
    QPointer<QNetworkAccessManager> m_manager;
public:
    explicit HttpManager(QObject *parent = nullptr);
    QFuture<std::shared_ptr<QNetworkReply>> post(const QNetworkRequest& request, const QByteArray &data);
    QFuture<std::shared_ptr<QNetworkReply>> get(const QNetworkRequest& request);
protected:
    virtual void customEvent(QEvent *event) override;
};

