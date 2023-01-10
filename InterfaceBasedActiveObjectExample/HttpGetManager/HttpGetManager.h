#pragma once
#include <QNetworkAccessManager>
#include <QPointer>
#include <QPromise>
#include <QFuture>
#include <QEvent>

class HttpGetManager : public QObject {
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
    QPointer<QNetworkAccessManager> m_manager;
public:
    explicit HttpGetManager(QObject *parent = nullptr);
    QFuture<std::shared_ptr<QNetworkReply>> get(const QNetworkRequest& request);
protected:
    virtual void customEvent(QEvent *event) override;
};

