#include "AsyncQDebugPrinter/AsyncQDebugPrinter.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCoreApplication>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    qDebug() << "Start application";
    auto printer = new AsyncQDebugPrinter{ qApp };
    printer->start();

    printer->print("Hello, world!").then(QtFuture::Launch::Async, [printer] {
        qDebug() << "In continuation";
        printer->print("Previous message was printed");
    });
    return app.exec();
}
