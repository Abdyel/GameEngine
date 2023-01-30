#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <string.h>
#include <vector>

//Enumeration that names the type of log that the Log entry is a part of
enum LogType {
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
};

//structure that define a log entry. given a log type and a message
struct LogEntry {
	LogType type;
	std::string message;
};

class Logger {
public:
	static std::vector<LogEntry> messages;//vector of messages that holds all logs used in a data structure.
	static void Log(const std::string& msg);
	static void Err(const std::string& msg);
private:
	static std::string CurrentDateTimeToString();
};

#endif