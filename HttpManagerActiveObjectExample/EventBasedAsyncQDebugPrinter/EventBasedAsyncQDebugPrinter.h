#pragma once
#include <QPromise>
#include <QFuture>
#include <QString>
#include <QObject>
#include <QEvent>


class EventBasedAsyncQDebugPrinter : public QObject {
private:
    Q_OBJECT
    class PrinterMessageEvent : public QEvent {
    private:
        QPromise<void> m_promise;
        const QString m_message;
    public:
        inline static constexpr QEvent::Type Type = static_cast<QEvent::Type>(QEvent::Type::User + 1);
        PrinterMessageEvent(const QString &message);
        const QString& message() const;
        QPromise<void>& promise();
    };
public:
    explicit EventBasedAsyncQDebugPrinter(QObject *parent = nullptr);
    QFuture<void> print(const QString& message) ;
protected:
    virtual void customEvent(QEvent *event) override;
};
