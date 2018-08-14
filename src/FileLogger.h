#pragma once

#include "spdlog/sinks/rotating_file_sink.h"

#include <QObject>
#include <QStandardPaths>

#include "logger.h"

class FileLogger : public QObject {
    Q_OBJECT
public:
    explicit FileLogger(QObject* parent = nullptr,
        const QString& logPath = QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)).arg("logs"),
        const size_t logSize = 1024 * 1024,
        const size_t logCount = 5);
    ~FileLogger();
    FileLogger(const FileLogger&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;
    FileLogger(FileLogger&&) = delete;
    FileLogger& operator=(FileLogger&&) = delete;

signals:

public slots:

private slots:
    void addLogMessage(const Logger::Message& message);

private:
    QString m_logPath;
    size_t m_logSize;
    size_t m_logCount;

    std::shared_ptr<spdlog::logger> m_logger;
};
