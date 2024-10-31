#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <QString>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QSettings>

enum LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static void setLogLevel(LogLevel level);
    static void log(LogLevel level, const QString& message);
    static void setLogFilePath(const QString& path);
    static void initFromConfig(const QString& configFilePath);
private:
    static LogLevel currentLogLevel;
    static QString logFilePath;
    static QString levelToString(LogLevel level);
    static QFile& getLogFile();
};


#endif // LOGGER_H

