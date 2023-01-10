#include "SignalBasedAsyncQDebugPrinter.h"
#include <QDebug>


SignalBasedAsyncQDebugPrinter::PrinterMessage::PrinterMessage(const QString &message)
    :m_message{ message } {}
const QString& SignalBasedAsyncQDebugPrinter::PrinterMessage::message() const {
    return m_message;
}
QPromise<void> &SignalBasedAsyncQDebugPrinter::PrinterMessage::promise() {
    return m_promise;
}


SignalBasedAsyncQDebugPrinter::SignalBasedAsyncQDebugPrinter(QObject *parent)
    :QObject{ parent } {
    QObject::connect(this, &SignalBasedAsyncQDebugPrinter::nextMessageReceived, this, &SignalBasedAsyncQDebugPrinter::handleNextMessage, Qt::ConnectionType::QueuedConnection);
}

QFuture<void> SignalBasedAsyncQDebugPrinter::print(const QString &message) {
    auto task = PrinterMessage{ message };
    auto future = task.promise().future();
    m_messages.emplace(std::move(task));
    emit this->nextMessageReceived();
    return future;
}
void SignalBasedAsyncQDebugPrinter::handleNextMessage() {
    while(not m_messages.empty()) {
        qDebug() << m_messages.front().message();
        m_messages.front().promise().finish();
        m_messages.pop();
    }
}
