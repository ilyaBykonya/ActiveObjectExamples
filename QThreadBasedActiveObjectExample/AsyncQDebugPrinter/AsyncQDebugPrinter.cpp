#include "AsyncQDebugPrinter.h"
#include <QDebug>
using namespace std::chrono_literals;

AsyncQDebugPrinter::PrinterMessage::PrinterMessage(const QString &message)
    :m_message{ message } {}
const QString& AsyncQDebugPrinter::PrinterMessage::message() const {
    return m_message;
}
QPromise<void> &AsyncQDebugPrinter::PrinterMessage::promise() {
    return m_promise;
}


AsyncQDebugPrinter::AsyncQDebugPrinter(QObject *parent)
    :QThread{ parent } {}
QFuture<void> AsyncQDebugPrinter::print(const QString &message) {
    auto task = PrinterMessage{ message };
    auto future = task.promise().future();
    std::lock_guard locker{ m_mutex };
    m_messages.push(std::move(task));
    m_messagesAwaiter.notify_one();
    return future;
}
void AsyncQDebugPrinter::run() {
    while(not isInterruptionRequested()) {
        std::unique_lock<std::mutex> locker{ m_mutex };
        if(m_messagesAwaiter.wait_until(locker, std::chrono::steady_clock::now() + 500ms, [this]() ->bool { return not m_messages.empty(); })) {
            auto buffer = std::move(m_messages);
            locker.unlock();

            while(not buffer.empty()) {
                qDebug() << buffer.front().message();
                buffer.front().promise().finish();
                buffer.pop();
            }
        }
    }
}
