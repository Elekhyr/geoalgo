#pragma once
#include <sstream>
#include <mutex>
#include <fstream>

// thanks to http://www.vilipetek.com/2014/04/17/thread-safe-simple-logger-in-c11/

// log message levels
enum Level	{ Finest, Finer, Fine, Config, Info, Warning, Severe };
class logger;


class logstream : public std::ostringstream
{
public:
	logstream(logger& oLogger, Level nLevel);
	logstream(const logstream& ls);
	~logstream();

private:
	logger& m_oLogger;
	Level m_nLevel;
};

class logger
{
public:
	logger(std::string filename);
	virtual ~logger();

	void log(Level nLevel, std::string oMessage);

	logstream operator()();
	logstream operator()(Level nLevel);

private:
	const tm* getLocalTime();

	std::mutex		m_oMutex;
	std::ofstream	m_oFile;

	tm				m_oLocalTime;
};

static logger olog("render.log");