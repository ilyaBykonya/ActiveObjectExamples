#include "EventBasedQDebugPrinter/EventBasedQDebugPrinter.h"
#include "MessagesAccumulator/MessagesAccumulator.h"
#include "HttpGetManager/HttpGetManager.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCoreApplication>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    auto printer = std::make_shared<MessagesAccumulator>(std::make_shared<EventBasedQDebugPrinter>(), std::chrono::milliseconds{ 2'000 });
    auto httpManager = std::make_shared<HttpGetManager>();

    QList tasks{
        printer->print("message_1"),
        printer->print("message_2"),
        printer->print("message_3")
    };
    QtFuture::whenAll(tasks.begin(), tasks.end()).
    then(QtFuture::Launch::Async, [printer, httpManager](const auto&/*results*/) {
        httpManager->get(QNetworkRequest{ QUrl{ "https://doc.qt.io/qt-6/qfuture.html" } }).
        then(QtFuture::Launch::Async, [printer](std::shared_ptr<QNetworkReply> reply) {
            return printer->print("Page loaded");
        }).unwrap().then(QtFuture::Launch::Async, [printer]{
            printer->print("All tasks completed");
        });
    });

    return app.exec();
}







/*
#include "EventBasedQDebugPrinter/EventBasedQDebugPrinter.h"
#include "MessagesAccumulator/MessagesAccumulator.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCoreApplication>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    auto printer = std::make_shared<MessagesAccumulator>(std::make_shared<EventBasedQDebugPrinter>(), std::chrono::milliseconds{ 2'000 });


    QList tasks{
        printer->print("message_1"),
        printer->print("message_2"),
        printer->print("message_3")
    };
    QtFuture::whenAll(tasks.begin(), tasks.end()).
    then([printer](const auto& results) {
        auto networkAccessManager = std::shared_ptr<QNetworkAccessManager>{ new QNetworkAccessManager{} };
        auto reply = std::shared_ptr<QNetworkReply>{ networkAccessManager->get(QNetworkRequest{ QUrl{ "https://doc.qt.io/qt-6/qfuture.html" } }) };

        QtFuture::connect(reply.get(), &QNetworkReply::finished).
        then([networkAccessManager, reply, printer] {
            return printer->print("Page loaded");
        }).unwrap().then([printer]{
            printer->print("All tasks completed");
        });
    });

    return app.exec();
}


*/

