#include "SignalBasedAsyncQDebugPrinter/SignalBasedAsyncQDebugPrinter.h"
#include "EventBasedAsyncQDebugPrinter/EventBasedAsyncQDebugPrinter.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCoreApplication>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    auto printerThread = new QThread{ qApp };
    auto printer = new SignalBasedAsyncQDebugPrinter{};
    printer->moveToThread(printerThread);
    printerThread->start();


    auto networkAccessManager = new QNetworkAccessManager{ qApp };
    auto reply = std::shared_ptr<QNetworkReply>{ networkAccessManager->get(QNetworkRequest{ QUrl{ "https://doc.qt.io/qt-6/qfuture.html" } }) };
    auto loadFuture = QtFuture::connect(reply.get(), &QNetworkReply::finished).
    then([reply, printer] {
        printer->print("Page loaded");
    });

    QList tasks{
        printer->print("message_1"),
        printer->print("message_2"),
        printer->print("message_3"),
        loadFuture
    };
    QtFuture::whenAll(tasks.begin(), tasks.end()).
    then(QtFuture::Launch::Async,
    [printer](const auto&/*results*/) {
        printer->print("All tasks completed");
    });

    return app.exec();
}
