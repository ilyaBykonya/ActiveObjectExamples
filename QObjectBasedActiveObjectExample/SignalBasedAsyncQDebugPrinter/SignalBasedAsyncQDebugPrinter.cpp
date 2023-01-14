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
    QObject::connect(this, &SignalBasedAsyncQDebugPrinter::nextMessageReceived, &SignalBasedAsyncQDebugPrinter::handleNextMessage);
}

QFuture<void> SignalBasedAsyncQDebugPrinter::print(const QString &message) {
    auto task = PrinterMessage{ message };
    auto future = task.promise().future();
    emplaceTaskInQueue(std::move(task));
    emit this->nextMessageReceived();
    return future;
}
void SignalBasedAsyncQDebugPrinter::handleNextMessage() {
    auto buffer{ takeTaskQueue() };
    while(not buffer.empty()) {
        qDebug() << buffer.front().message();
        buffer.front().promise().finish();
        buffer.pop();
    }
}

void SignalBasedAsyncQDebugPrinter::emplaceTaskInQueue(PrinterMessage &&task) {
    std::lock_guard locker{ m_mutex };
    m_messages.emplace(std::move(task));
}

std::queue<SignalBasedAsyncQDebugPrinter::PrinterMessage> SignalBasedAsyncQDebugPrinter::takeTaskQueue() {
    std::queue<PrinterMessage> buffer;
    std::lock_guard locker{ m_mutex };
    m_messages.swap(buffer);
    return buffer;
}

