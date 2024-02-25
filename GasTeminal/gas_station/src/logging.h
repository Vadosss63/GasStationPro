#pragma once

#include <QString>

#include "logger_defs.h"

void printLog(LogLevel logLevel, const QString& msg, const QString& funcName = "");

#define LOG_DEBUG(msg) printLog(LogLevel::DEBUG, msg, __func__)
#define LOG_INFO(msg) printLog(LogLevel::INFO, msg, __func__)
#define LOG_WARNING(msg) printLog(LogLevel::WARNING, msg, __func__)
#define LOG_ERROR(msg) printLog(LogLevel::ERROR, msg, __func__)

void printLogInf(const QString& log);

void printLogInf(const QByteArray& data);

void printLogErr(const QString& log);

QString convertToString(const QByteArray& data);
