#pragma once
#include <set>
#include <unistd.h>
#include <map>
#include <memory>
#include <vector>

#include "programstats.hpp"


class progmanager_library
{
public:
	progmanager_library() = default;

	void update();
	std::vector<std::pair<double, pid_t>> highest_utilization(int top_n = 5);

	const std::unique_ptr<programstats> &prog(pid_t pid) const;


private:
	static std::set<pid_t> get_pids() ;



	std::set<pid_t> pids_;
	std::map<pid_t, std::unique_ptr<programstats>> progs_;
};

