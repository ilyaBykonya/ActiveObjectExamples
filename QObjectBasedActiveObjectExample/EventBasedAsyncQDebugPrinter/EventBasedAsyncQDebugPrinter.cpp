#include "EventBasedAsyncQDebugPrinter.h"
#include <QCoreApplication>

EventBasedAsyncQDebugPrinter::PrinterMessageEvent::PrinterMessageEvent(const QString &message)
    :QEvent{ Type }, m_message{ message } {}
const QString &EventBasedAsyncQDebugPrinter::PrinterMessageEvent::message() const {
    return m_message;
}
QPromise<void> &EventBasedAsyncQDebugPrinter::PrinterMessageEvent::promise() {
    return m_promise;
}



EventBasedAsyncQDebugPrinter::EventBasedAsyncQDebugPrinter(QObject *parent)
    :QObject{ parent } {}
QFuture<void> EventBasedAsyncQDebugPrinter::print(const QString &message) {
    auto task = new PrinterMessageEvent{ message };
    auto future = task->promise().future();
    qApp->postEvent(this, task);
    return future;
}
void EventBasedAsyncQDebugPrinter::customEvent(QEvent *event) {
    if(auto message = dynamic_cast<PrinterMessageEvent*>(event); message) {
        qDebug() << message->message();
        message->promise().finish();
    }
}

