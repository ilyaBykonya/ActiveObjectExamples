#include "EventBasedAsyncQDebugPrinter/EventBasedAsyncQDebugPrinter.h"
#include "HttpManager/HttpManager.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCoreApplication>
#include <QScopedPointer>
#include <QTimer>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    auto printer = std::make_shared<EventBasedAsyncQDebugPrinter>();
    auto httpManager = std::make_shared<HttpManager>();

    auto printerThread = new QThread{};
    printer->moveToThread(printerThread);
    printerThread->start();

    auto httpThread = new QThread{};
    httpManager->moveToThread(httpThread);
    httpThread->start();

    httpManager->post(QNetworkRequest{ QUrl{ "https://habr.com/ru/post/710368/" } }, {}).
    then(QtFuture::Launch::Async, [printer](std::shared_ptr<QNetworkReply> reply) {
        printer->print(reply->readAll());
    });

    return app.exec();
}


