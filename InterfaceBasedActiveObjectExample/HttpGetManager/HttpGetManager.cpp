#include "HttpGetManager.h"
#include <QCoreApplication>
#include <QNetworkRequest>
#include <QNetworkReply>

HttpGetManager::HttpGetManager(QObject *parent)
    :QObject{ parent }, m_manager{ new QNetworkAccessManager{ this } } {}

QFuture<std::shared_ptr<QNetworkReply>> HttpGetManager::get(const QNetworkRequest& request) {
    auto task = new HttpGetRequestEvent{ std::move(request) };
    auto future = task->promise().future();
    qApp->postEvent(this, task);
    return future;
}

void HttpGetManager::customEvent(QEvent *event) {
    if(auto request = dynamic_cast<HttpGetRequestEvent*>(event); request) {
        auto reply = std::shared_ptr<QNetworkReply>{ m_manager->get(request->request()) };
        QtFuture::connect(reply.get(), &QNetworkReply::finished).
        then([reply, reply_promise = std::move(request->promise())]() mutable ->void {
            reply_promise.addResult(reply);
            reply_promise.finish();
        });
    }
}


HttpGetManager::HttpGetRequestEvent::HttpGetRequestEvent(const QNetworkRequest &request)
    :QEvent{ Type }, m_request{ std::move(request) } {}
QPromise<std::shared_ptr<QNetworkReply>> &HttpGetManager::HttpGetRequestEvent::promise() {
    return m_promise;
}
const QNetworkRequest &HttpGetManager::HttpGetRequestEvent::request() const {
    return m_request;
}
