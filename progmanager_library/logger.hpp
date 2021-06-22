#pragma once

#ifndef LOG_ERROR
#define LOG_ERROR(msg){try{ logger::log_error(std::string() + msg);} catch(...){}}
#endif

#ifndef LOG_INFO
#define LOG_INFO(msg){try{ logger::log_info(std::string() + msg);} catch(...){}}
#endif

#ifndef LOG_LINE_BEGIN
#define LOG_LINE_BEGIN(msg){try{ logger::log_line_begin(std::string() + msg);}catch(...){}}
#endif

#ifndef LOG_LINE_NEXT
#define LOG_LINE_NEXT(msg){try{ logger::log_line_next(std::string() + msg);}catch(...){}}
#endif


#include <string>


class logger
{
public:
	logger();
	~logger();

	static void log_error(std::string msg);
	static void log_info(std::string msg);

	static void log_line_begin(std::string msg);
	static void log_line_next(std::string msg);
	
private:
	static int lines_;
};

