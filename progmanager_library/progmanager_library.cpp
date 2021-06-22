#include "progmanager_library.hpp"

#include <dirent.h>
#include <stdexcept>
#include <algorithm>


#include "logger.hpp"
#include <cstring>


void progmanager_library::update()
{
	//Get new set of pids and check for differences.
	try{
		auto pids_new = get_pids();
		std::set<pid_t> obsoletes;
		std::set<pid_t> newones;
		std::set_difference(pids_.begin(), pids_.end(), pids_new.begin(), pids_new.end(), std::inserter(obsoletes,obsoletes.begin()));
		std::set_difference(pids_new.begin(), pids_new.end(), pids_.begin(), pids_.end(), std::inserter(newones, newones.begin()));

		//Remove obsolete pids that are no longer used.
		for(auto pid : obsoletes){
			progs_.erase(pid);
			//LOG_INFO("deleted " + std::to_string(pid));
		}
		//Add new pids.
		for(auto pid: newones){
			progs_.emplace(pid, std::make_unique<programstats>(pid));
			//LOG_INFO("added   " + std::to_string(pid));
		}
		//Check if error occurred.
		if(progs_.size() != pids_new.size()){
			LOG_ERROR("SIZE MISSMATCH");
			LOG_INFO("old size: " + std::to_string(progs_.size()))
			LOG_INFO("new size: " + std::to_string(pids_new.size()));
		}
		pids_ = pids_new;

		//Update all cpu usage stats
		for (const auto& prog : progs_)	{
			prog.second->update();
		}
	}
	catch (const std::runtime_error& e) {
		LOG_ERROR("Unable to update programms: " + e.what());
	}
}

std::vector<std::pair<double, pid_t>> progmanager_library::highest_utilization(int top_n)
{
	std::vector<std::pair<double, pid_t>> usages;
	usages.reserve(progs_.size());

	for (const auto& prog : progs_)
		usages.emplace_back(prog.second->cpu_percent_user(), prog.first);

	std::sort(usages.begin(), usages.end(),
		[&](const std::pair<double, pid_t> &first, const std::pair<double, pid_t> &second) ->bool {
		return first.first > second.first; });

	return { usages.begin(), usages.begin() + top_n };
}

const std::unique_ptr<programstats>& progmanager_library::prog(pid_t pid) const
{
	return progs_.at(pid);
}


std::set<pid_t> progmanager_library::get_pids()
{
	const char* PATH = "/proc";
	DIR *dir = opendir(PATH);
	if (!dir) throw std::runtime_error("unable to open proc");
	struct dirent *entry = readdir(dir);

	std::set<pid_t> pids;

	while (entry != nullptr) {
		if (entry->d_type == DT_DIR) {
			try { pids.emplace(std::stoi(entry->d_name)); }
			//throws if convertion to int fails
			catch (const std::invalid_argument& e) {};
		}
		entry = readdir(dir);
	}
	closedir(dir);

	return pids;
}
