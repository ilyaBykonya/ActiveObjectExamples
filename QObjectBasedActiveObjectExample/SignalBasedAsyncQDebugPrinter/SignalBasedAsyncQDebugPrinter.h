#pragma once
#include <QPromise>
#include <QFuture>
#include <QString>
#include <QObject>
#include <queue>

class SignalBasedAsyncQDebugPrinter : public QObject {
private:
    Q_OBJECT
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
    std::mutex m_mutex;
public:
    explicit SignalBasedAsyncQDebugPrinter(QObject *parent = nullptr);
    QFuture<void> print(const QString& message);
private slots:
    void handleNextMessage();
signals:
    void nextMessageReceived();
private:
    void emplaceTaskInQueue(PrinterMessage&& task);
    std::queue<PrinterMessage> takeTaskQueue();
};

