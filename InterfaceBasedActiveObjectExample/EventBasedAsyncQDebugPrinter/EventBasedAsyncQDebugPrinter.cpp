#include "EventBasedAsyncQDebugPrinter.h"
#include <QCoreApplication>

EventBasedAsyncQDebugPrinter::PrinterTaskEvent::PrinterTaskEvent(const QString &message)
    :QEvent{ Type }, PrinterTask{ message } {}

EventBasedAsyncQDebugPrinter::EventBasedAsyncQDebugPrinter(QObject *parent)
    :QObject{ parent } {}
QFuture<void> EventBasedAsyncQDebugPrinter::print(const QString &message) {
    auto task = new PrinterTaskEvent{ message };
    auto future = task->promise().future();
    qApp->postEvent(this, task);
    return future;
}
void EventBasedAsyncQDebugPrinter::customEvent(QEvent *event) {
    if(auto message = dynamic_cast<PrinterTaskEvent*>(event); message) {
        qDebug() << message->message();
        message->promise().finish();
    }
}

