//
// Created by nano on 11.03.21.
//

#include "powerlogger.hpp"
#include <fstream>


#include "../progmanager_library/logger.hpp"

powerlogger::powerlogger(std::string path, const std::string& header) :
	path_(std::move(path))
{
	if(!header.empty()){
		try{
			std::ofstream file(path_, std::ios_base::out | std::ios_base::trunc);
			file << header << std::endl;
			//LOG_INFO("file: " + path_ + " header: " + header);
			file.flush();
			file.close();
		}
		catch (const std::ios_base::failure &e) {
			LOG_ERROR("Unable to write header to " + path_ + ": " + e.what());
		}
	}
}

powerlogger::~powerlogger() {
	flush();
}

void powerlogger::flush() {
	try{
		std::ofstream file(path_, std::ios_base::app | std::ios_base::out);
		while(!buffer_.empty()){
			file << buffer_.front() << std::endl;
			buffer_.pop_front();
		}
		file.flush();
		file.close();
	}
	catch (const std::ios_base::failure &e) {
		LOG_ERROR("Unable to write log to " + path_ + ": " + e.what());
	}
}

void powerlogger::log(const std::string &line) {
	buffer_.emplace_back(line);
	if(buffer_.size() >= buffersize) flush();
}
