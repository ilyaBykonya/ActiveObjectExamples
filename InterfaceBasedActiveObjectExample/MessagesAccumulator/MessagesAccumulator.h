#pragma once
#include "../IPrinter/IPrinter.h"
#include <QObject>
#include <queue>

class MessagesAccumulator : public QObject, public IPrinter {
    Q_OBJECT
private:
    std::shared_ptr<IPrinter> m_next;
    std::queue<PrinterTask> m_buffer;
private:
    std::mutex m_mutex;
public:
    MessagesAccumulator(std::shared_ptr<IPrinter> next, std::chrono::milliseconds delay, QObject* parent = nullptr);
    virtual QFuture<void> print(const QString &message) override;
private slots:
    void flushMessagesQueue();
};

