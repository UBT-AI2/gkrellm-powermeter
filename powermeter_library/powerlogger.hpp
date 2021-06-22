//
// Created by nano on 11.03.21.
//

#ifndef GKRELLM_POWERMETER_POWERLOGGER_HPP
#define GKRELLM_POWERMETER_POWERLOGGER_HPP


#include <string>
#include <list>

class powerlogger{
	std::list<std::string> buffer_;
	static constexpr int buffersize = 60;
	static constexpr const char* default_path = "powerlog.csv";

public:
	explicit powerlogger(
			std::string path = default_path,
			const std::string& header = "");
	~powerlogger();
	void flush();
	void log(const std::string &line);

	const std::string path_;
};


#endif //GKRELLM_POWERMETER_POWERLOGGER_HPP
