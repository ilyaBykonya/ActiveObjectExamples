#include "EventBasedQDebugPrinter.h"
#include <QCoreApplication>

EventBasedQDebugPrinter::PrinterTaskEvent::PrinterTaskEvent(const QString &message)
    :QEvent{ Type }, PrinterTask{ message } {}

EventBasedQDebugPrinter::EventBasedQDebugPrinter(QObject *parent)
    :QObject{ parent } {}
QFuture<void> EventBasedQDebugPrinter::print(const QString &message) {
    auto task = new PrinterTaskEvent{ message };
    auto future = task->promise().future();
    qApp->postEvent(this, task);
    return future;
}
void EventBasedQDebugPrinter::customEvent(QEvent *event) {
    if(auto message = dynamic_cast<PrinterTaskEvent*>(event); message) {
        qDebug() << message->message();
        message->promise().finish();
    }
}

