#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

// needed for MSVC
#define localtime_r(_Time, _Tm) localtime_s(_Tm, _Time)

// Convert date and time info from tm to a character string
// in format "YYYY-mm-DD HH:MM:SS" and send it to a stream
std::ostream& operator<< (std::ostream& stream, const tm* tm)
{
	// I had to muck around this section since GCC 4.8.1 did not implement std::put_time
	// return stream << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
	return stream << 1900 + tm->tm_year << '-' <<
		std::setfill('0') << std::setw(2) << tm->tm_mon + 1 << '-'
		<< std::setfill('0') << std::setw(2) << tm->tm_mday << ' '
		<< std::setfill('0') << std::setw(2) << tm->tm_hour << ':'
		<< std::setfill('0') << std::setw(2) << tm->tm_min << ':'
		<< std::setfill('0') << std::setw(2) << tm->tm_sec;
}

logger::logger(std::string filename)
{
	m_oFile.open(filename, std::fstream::out | std::fstream::app | std::fstream::ate);
}

logger::~logger()
{
	m_oFile.flush();
	m_oFile.close();
}

logstream logger::operator()()
{
	return logstream(*this, Info);
}

logstream logger::operator()(Level nLevel)
{
	return logstream(*this, nLevel);
}

const tm* logger::getLocalTime()
{
	auto in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	localtime_s(&m_oLocalTime, &in_time_t);
	return &m_oLocalTime;
}

void logger::log(Level nLevel, std::string oMessage)
{
	const static char* LevelStr[] = { "Finest", "Finer", "Fine", "Config", "Info", "Warning", "Severe" };

	m_oMutex.lock();
	m_oFile << '[' << getLocalTime() << ']'
		<< '[' << LevelStr[nLevel] << "]\t"
		<< oMessage << std::endl;
	m_oMutex.unlock();
}

logstream::logstream(logger& oLogger, Level nLevel) :
m_oLogger(oLogger), m_nLevel(nLevel)
{
}

logstream::logstream(const logstream& ls) :
m_oLogger(ls.m_oLogger), m_nLevel(ls.m_nLevel)
{
	// As of GCC 8.4.1 basic_stream is still lacking a copy constructor
	// (part of C++11 specification)
	//
	// GCC compiler expects the copy constructor even thought because of
	// RVO this constructor is never used
}

logstream::~logstream()
{
	m_oLogger.log(m_nLevel, str());
}
