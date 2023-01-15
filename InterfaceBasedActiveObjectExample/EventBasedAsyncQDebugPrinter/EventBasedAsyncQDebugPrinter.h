#pragma once
#include "../IPrinter/IPrinter.h"
#include <QPromise>
#include <QFuture>
#include <QString>
#include <QObject>
#include <QEvent>


class EventBasedAsyncQDebugPrinter : public QObject, public IPrinter {
private:
    Q_OBJECT
    class PrinterTaskEvent : public QEvent, public PrinterTask {
    public:
        inline static constexpr QEvent::Type Type = static_cast<QEvent::Type>(QEvent::Type::User + 1);
        PrinterTaskEvent(const QString &message);
    };
public:
    explicit EventBasedAsyncQDebugPrinter(QObject *parent = nullptr);
    virtual QFuture<void> print(const QString& message) override;
public:
    void setPriority(qint32 newPriority);
    qint32 priority() const;
protected:
    virtual void customEvent(QEvent *event) override;
};



