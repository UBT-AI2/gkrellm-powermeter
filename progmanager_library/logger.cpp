#include "logger.hpp"

int logger::lines_ = -1;

logger::logger()
{
}


logger::~logger()
{
}

void logger::log_error(std::string msg)
{
	lines_ = 0;
	printf("\033[1;31m%s\033[0m\n", msg.c_str());
}

void logger::log_info(std::string msg)
{
	lines_ = 0;
	printf("\033[35m%s\033[0m\n", msg.c_str());
}

void logger::log_line_begin(std::string msg)
{
	//lines_ = 1;
	//Jump to linebeginning, jump @lines_ lines up, clear line, print message, jump to next line
	printf("\r\x1b[%dA\33[2K%s\n", lines_, msg.c_str());
	
	lines_ = 1;
}

void logger::log_line_next(std::string msg)
{
	++lines_;
	//Clear current line, then jump to next line
	printf("\33[2K%s\n", msg.c_str());
}
