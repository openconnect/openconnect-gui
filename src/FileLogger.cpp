#include "FileLogger.h"

#include <QDir>
#include <QApplication>
#include <QDateTime>
#include <QMessageBox>

namespace spd = spdlog;

FileLogger::FileLogger(QObject *parent, const QString& logPath, const size_t logSize, const size_t logCount) :
    QObject(parent),
    m_logPath{logPath},
    m_logSize{logSize},
    m_logCount{logCount}
{

    QDir dir;
    dir.mkpath(m_logPath);
    try {
        m_logger = spd::rotating_logger_mt("openconnect-logger",
#ifdef Q_OS_WIN
                                           QString("%1/%2").arg(m_logPath).arg(qApp->applicationName()).toStdWString(),
#else
                                           QString("%1/%2").arg(m_logPath).arg(qApp->applicationName()).toStdString(),
#endif
                                           logSize,
                                           logCount);
        spd::set_pattern("%v");
    } catch (const spd::spdlog_ex& ex) {
        QMessageBox::critical(nullptr,
                              QString("Log file init failed"),
                              QString(ex.what()));
    }

    connect(&Logger::instance(), &Logger::newLogMessage, this, &FileLogger::addLogMessage);
}

FileLogger::~FileLogger()
{
    Logger::instance().addMessage(QString("...logging finished"));
}

void FileLogger::addLogMessage(const Logger::Message& message)
{
    QDateTime dt;
    dt.setMSecsSinceEpoch(message.timeStamp);

    m_logger->log(spdlog::level::off,
                  "{:<24} | {:>4} | {}",
                  dt.toString("yyyy-MM-dd hh:mm:ss.ms").toStdString(),
                  QString::number((long long)message.threadId, 16).toStdString(),
                  message.text.toStdString());
    m_logger->flush();
}
