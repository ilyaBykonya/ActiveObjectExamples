#pragma once
#include <QPromise>
#include <QThread>
#include <QString>
#include <QFuture>
#include <queue>

class AsyncQDebugPrinter : public QThread {
private:
    class PrinterMessage {
    private:
        QPromise<void> m_promise;
        const QString m_message;
    public:
        PrinterMessage(const QString &message);
        const QString& message() const;
        QPromise<void>& promise();
    };
private:
    std::queue<PrinterMessage> m_messages;
    std::condition_variable m_messagesAwaiter;
    std::mutex m_mutex;
public:
    explicit AsyncQDebugPrinter(QObject *parent = nullptr);
    QFuture<void> print(const QString& message);
protected:
    virtual void run() override;
};

