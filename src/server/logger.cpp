#include "logger.h"

// DEFAULT
LogLevel Logger::currentLogLevel = INFO;
QString Logger::logFilePath = "log_server.txt";

void Logger::setLogLevel(LogLevel level) {
    currentLogLevel = level;
}

QFile& Logger::getLogFile() {
    static QFile logFile(logFilePath);
    return logFile;
}


void Logger::setLogFilePath(const QString& path) {
    logFilePath = path;
    getLogFile().close();
    getLogFile().open(QIODevice::WriteOnly | QIODevice::Text);
}

void Logger::initFromConfig(const QString& configFilePath) {
    const QFileInfo configFileInfo(configFilePath);
    if (!configFileInfo.exists() or configFileInfo.size() == 0) {
        Logger::log(ERROR, "Config file does not exist or empty, default values to logger were set");
        return;
    }

    const QSettings settings(configFilePath, QSettings::IniFormat);

    const QString logLevelStr = settings.value("Logging/LogLevel").toString();
    const QString logPath = settings.value("Logging/LogPath").toString();

    LogLevel level;
    if (logLevelStr == "DEBUG") {
        level = DEBUG;
    } else if (logLevelStr == "INFO") {
        level = INFO;
    } else if (logLevelStr == "WARNING") {
        level = WARNING;
    } else if (logLevelStr == "ERROR") {
        level = ERROR;
    } else {
        Logger::log(WARNING, "Invalid log level: " + logLevelStr + ", INFO logLevel selected");
        level = INFO;
    }

    setLogLevel(level);
    setLogFilePath(logPath);

    Logger::log(INFO, "Got logLevel and logPath from config to logger");
}

void Logger::log(LogLevel level, const QString& message) {
    if (level >= currentLogLevel) {
        qDebug() << levelToString(level) << ": " << message;
        QTextStream stream(&getLogFile());
        stream << levelToString(level) << ": " << message << Qt::endl;
    }
}

QString Logger::levelToString(LogLevel level) {
    switch (level) {
    case DEBUG:
        return "DEBUG";
    case INFO:
        return "INFO";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}
