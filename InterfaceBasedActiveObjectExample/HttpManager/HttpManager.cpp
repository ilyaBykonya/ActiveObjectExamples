#include "HttpManager.h"
#include <QCoreApplication>
#include <QNetworkRequest>
#include <QNetworkReply>

HttpManager::HttpManager(QObject *parent)
    :QObject{ parent }, m_manager{ new QNetworkAccessManager{ this } } {}

QFuture<std::shared_ptr<QNetworkReply>> HttpManager::post(const QNetworkRequest &request, const QByteArray &data) {
    auto task = new HttpPostRequestEvent{ std::move(request), std::move(data) };
    auto future = task->promise().future();
    qApp->postEvent(this, task);
    return future;
}
QFuture<std::shared_ptr<QNetworkReply>> HttpManager::get(const QNetworkRequest& request) {
    auto task = new HttpGetRequestEvent{ std::move(request) };
    auto future = task->promise().future();
    qApp->postEvent(this, task);
    return future;
}

void HttpManager::customEvent(QEvent *event) {
    std::shared_ptr<QNetworkReply> reply{};
    QPromise<std::shared_ptr<QNetworkReply>> requestPromise{};

    if(auto request = dynamic_cast<HttpGetRequestEvent*>(event); request) {
        reply = std::shared_ptr<QNetworkReply>{ m_manager->get(request->request()), QScopedPointerDeleteLater{} };
        requestPromise = std::move(request->promise());
    } else if(auto request = dynamic_cast<HttpPostRequestEvent*>(event); request) {
        reply = std::shared_ptr<QNetworkReply>{ m_manager->post(request->request(), request->data()), QScopedPointerDeleteLater{} };
        requestPromise = std::move(request->promise());
    }

    QtFuture::connect(reply.get(), &QNetworkReply::finished).
    then([reply, replyPromise = std::move(requestPromise)]() mutable ->void {
        replyPromise.addResult(reply);
        replyPromise.finish();
    });
}


HttpManager::HttpGetRequestEvent::HttpGetRequestEvent(const QNetworkRequest &request)
    :QEvent{ Type }, m_request{ std::move(request) } {}
QPromise<std::shared_ptr<QNetworkReply>> &HttpManager::HttpGetRequestEvent::promise() {
    return m_promise;
}
const QNetworkRequest &HttpManager::HttpGetRequestEvent::request() const {
    return m_request;
}



HttpManager::HttpPostRequestEvent::HttpPostRequestEvent(const QNetworkRequest &request, const QByteArray &data)
    :QEvent{ Type }, m_request{ std::move(request) }, m_data{ std::move(data) } {}
QPromise<std::shared_ptr<QNetworkReply> > &HttpManager::HttpPostRequestEvent::promise(){
    return m_promise;
}
const QNetworkRequest &HttpManager::HttpPostRequestEvent::request() const{
    return m_request;
}
const QByteArray &HttpManager::HttpPostRequestEvent::data() const{
    return m_data;
}
