#include "MessagesAccumulator.h"
#include <QPromise>
#include <QTimer>

MessagesAccumulator::MessagesAccumulator(std::shared_ptr<IPrinter> next, std::chrono::milliseconds delay, QObject* parent)
    :QObject{ parent },
     m_next{ std::move(next) } {
        auto timer = new QTimer{ this };
        QObject::connect(timer, &QTimer::timeout, this, &MessagesAccumulator::flushMessagesQueue);
        timer->start(delay);
    }

QFuture<void> MessagesAccumulator::print(const QString &message) {
    auto task = PrinterTask{ message };
    auto future = task.promise().future();
    std::lock_guard locker{ m_mutex };
    m_buffer.emplace(std::move(task));
    return future;
}

void MessagesAccumulator::flushMessagesQueue() {
    std::queue<PrinterTask> messages;
    {
        std::lock_guard locker{ m_mutex };
        messages.swap(m_buffer);
    }

    while(not messages.empty()) {
        auto task = std::move(messages.front());
        messages.pop();
        m_next->print(task.message()).then([promise = std::move(task.promise())]() mutable ->void {
            promise.finish();
        });
    }
}

