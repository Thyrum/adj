// This library requires c++11 or higher
// Made by:
//     Aron de Jong (s2120437)
// Using ideas/code snippets from
//     https://www.drdobbs.com/cpp/logging-in-c/201804215
//     
// This header-only library is made for easy logging
//
// Created on:
//      2020-08-29
// Last edit:
//      2020-08-29
#pragma once

#include <iostream>
#include <sstream>


namespace adj
{

constexpr const char *logHeaders[] =
{
	"[ERROR %T] ",
	"[WARN  %T] ",
	"[DEBUG %T] ",
	"[INFO  %T] ",
};

enum LogLevel
{
	logERROR = 0,
	logWARN,
	logDEBUG,
	logINFO,
};


template<typename OutputPolicy>
class Log
{
public:
	Log() = default;
	virtual ~Log();
	std::ostringstream& Get(LogLevel level = logINFO);

	static LogLevel& ReportingLevel();

protected:
	std::ostringstream os;

private:
	Log(const Log&);
	Log& operator=(const Log&);

	LogLevel messageLevel;
};

template<typename OutputPolicy>
Log<OutputPolicy>::~Log()
{
	os << std::endl;
	OutputPolicy::Output(os.str());
}

template<typename OutputPolicy>
std::ostringstream& Log<OutputPolicy>::Get(LogLevel level)
{
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), logHeaders[level], timeinfo);
	os << buffer;
	messageLevel = level;
	return os;
}

template<typename OutputPolicy>
LogLevel& Log<OutputPolicy>::ReportingLevel()
{
	static LogLevel lvl = logERROR;
	return lvl;
}

class Output2FILE // implementation of OutputPolicy
{
public:
	static FILE*& Stream();
	static void Output(const std::string& msg);
};

inline FILE*& Output2FILE::Stream()
{
	static FILE* pStream = stderr;
	return pStream;
}

inline void Output2FILE::Output(const std::string& msg)
{
	FILE* pStream = Stream();
	if (!pStream)
		return;

	fprintf(pStream, "%s", msg.c_str());
	fflush(pStream);
}

typedef Log<Output2FILE> FILELog;
#ifndef FILELOG_MAX_LEVEL
#define FILELOG_MAX_LEVEL logINFO
#endif
#define LOG(level) \
	if (level > FILELOG_MAX_LEVEL) ;\
	else if (level > FILELog::ReportingLevel() || !Output2FILE::Stream()) ; \
	else FILELog().Get(level)

}
