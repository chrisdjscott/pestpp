#include <iomanip>
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <cstring>
#include "logger.h"
#include "config_os.h"

using namespace std;
using std::chrono::high_resolution_clock;

void Logger::writetime(stringstream &os, time_t tc) {
	// alternative to put_time iomanip
	// as put_time is not implimented in gcc4.8
	locale loc;
	const time_put<char>& tp = use_facet<time_put<char>>(loc);
	const char *pat = "%F %T";
	tp.put(os, os, ' ', localtime(&tc), pat, pat + strlen(pat));
}

Logger::Logger(ofstream &_fout,bool _echo)
: fout(_fout),echo(_echo){}

void Logger::log(const string &message)
{	
	high_resolution_clock::time_point time_now = high_resolution_clock::now();
	map<string, chrono::high_resolution_clock::time_point>::iterator message_iter = tagged_events.find(message);
	//if this is a new message
	if (message_iter == tagged_events.end())
	{
		tagged_events[message] = time_now;
		fout << time_to_string(time_now) << "-> starting " << message << endl;
		if (echo)
			cout << time_to_string(time_now) << "-> starting " << message << endl;
	}
	else
	{
		high_resolution_clock::time_point time_start = message_iter->second;
		fout << time_to_string(time_now) << "-> finished " << message << ", elapsed time = " <<
			elapsed_time_to_string(time_now, time_start) << " )" << endl;
		if (echo)
			cout << time_to_string(time_now) << "-> finished " << message << ", elapsed time = " <<
				elapsed_time_to_string(time_now, time_start) << " )" << endl;
		tagged_events.erase(message_iter);
	}
}

string Logger::time_to_string(const std::chrono::high_resolution_clock::time_point &tmp_time)
{
	stringstream time_str;
	auto tmp_time_c = high_resolution_clock::to_time_t(tmp_time);
#ifdef OS_LINUX
	writetime(time_str, tmp_time_c);
#endif
#ifdef OS_WIN
	time_str << put_time(std::localtime(&tmp_time_c), "%X");
#endif
	return time_str.str();
}

string Logger::elapsed_time_to_string(std::chrono::high_resolution_clock::time_point &current_time, std::chrono::high_resolution_clock::time_point &prev_time)
{
	ostringstream str;
	auto delta_t = current_time - prev_time;
	if (delta_t < std::chrono::milliseconds(1))
	{
		str << std::chrono::duration_cast<std::chrono::microseconds>(delta_t).count() << "us";
	}
	else if (delta_t < std::chrono::seconds(1))
	{
		str << std::chrono::duration_cast<std::chrono::milliseconds>(delta_t).count() << "ms";
	}
	else if (delta_t < std::chrono::minutes(1))
	{
		str << std::chrono::duration_cast<std::chrono::seconds>(delta_t).count() << "sec";
	}
	else if (delta_t < std::chrono::hours(1))
	{
		str << std::chrono::duration_cast<std::chrono::minutes>(delta_t).count() << "min";
	}
	else
	{
		str << std::chrono::duration_cast<std::chrono::hours>(delta_t).count() << "hr";
	}
	return str.str();
}


Logger::~Logger()
{
}
