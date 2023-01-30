#include "Logger.h"
#include <iostream>
#include <string.h>
#include <chrono>
#include <ctime>

std::vector<LogEntry> Logger::messages;

//method below creates a string that formats the time and date into a neat string that will be used by logger class
std::string Logger::CurrentDateTimeToString() {
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::string output(30, '\0');

	struct tm timeinfo;
	localtime_s(&timeinfo, &now);

	std::strftime(&output[0], output.size(), "%d-%b-%Y %H:%M:%S", &timeinfo);
	return output;
}

void Logger::Log(const std::string& msg) {
	LogEntry logEntry;
	logEntry.type = LOG_INFO;
	logEntry.message = "LOG: [" + CurrentDateTimeToString() + "]: " + msg;

	std::cout << "\x1B[32m" << logEntry.message << "\033[0m" << std::endl;

	messages.push_back(logEntry);
}
void Logger::Err(const std::string& msg) {
	LogEntry logEntry;
	logEntry.type = LOG_ERROR;
	logEntry.message = "ERROR: [" + CurrentDateTimeToString() + "]: " + msg;

	std::cout << "\x1B[91m" << logEntry.message << "\033[0m" << std::endl;

	messages.push_back(logEntry);
}