#include "IPrinter.h"


PrinterTask::PrinterTask(const QString &message)
    :m_message{ message } {}
const QString& PrinterTask::message() const {
    return m_message;
}
QPromise<void>& PrinterTask::promise() {
    return m_promise;
}
