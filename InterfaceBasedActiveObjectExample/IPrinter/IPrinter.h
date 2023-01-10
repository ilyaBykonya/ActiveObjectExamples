#pragma once
#include <QPromise>
#include <QFuture>
#include <QString>

struct IPrinter {
    virtual QFuture<void> print(const QString& message) = 0;
    virtual ~IPrinter() = default;
};

class PrinterTask {
private:
    QPromise<void> m_promise;
    const QString m_message;
public:
    PrinterTask(const QString &message);
    const QString& message() const;
    QPromise<void>& promise();
};
