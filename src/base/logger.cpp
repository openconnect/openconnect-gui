#include "base/logger.h"

#include <QDateTime>

void Logger::addMessage(const QString& message, const MessageType& messageType, const ComponentType& componentType)
{
    QWriteLocker lock(&m_lock);

    Message tmp{QDateTime::currentMSecsSinceEpoch(),
                messageType,
                componentType,
                message,
                ++m_messageCounter
               };
    m_messages.push_back(tmp);

    if (m_messages.size() >= 20000) {// TODO: magic constant
        m_messages.pop_front();
    }

    emit newLogMessage(tmp);
}

QVector<Logger::Message> Logger::getMessages(int lastKnownId) const
{
    QReadLocker lock(&m_lock);

    int diff{m_messageCounter - lastKnownId};
    int size{m_messages.size()};

    if (lastKnownId == -1 || diff >= size) {
        return m_messages;
    }

    if (diff <= 0) {
        return QVector<Message>();
    }

    return m_messages.mid(size - diff);
}

Logger::Logger(QObject *parent) : QObject(parent),
    m_messageCounter{-1},
    m_lock{QReadWriteLock::Recursive}
{
}
