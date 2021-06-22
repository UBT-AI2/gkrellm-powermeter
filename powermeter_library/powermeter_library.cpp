#include "powermeter_library.h"

#include "rapl_reader.h"
#include <memory>
#include <cstring>
#include "powerlogger.hpp"
#include "../progmanager_library/logger.hpp"

extern "C" {

static std::shared_ptr<rapl_reader> rapl_reader_;
static std::vector<std::shared_ptr<rapl_reader>> all_readers_;
//pairs of path_to_energy_uj and powertarget name
static std::vector<std::pair<std::string, std::string>> power_targets_;
static std::unique_ptr<powerlogger> powerlogger_;

void powermeter_library_init(const char* logpath) {
	power_targets_ = rapl_reader::power_targets();
	std::string header;
	for(auto const &p : power_targets_){
		all_readers_.push_back(std::make_shared<rapl_reader>(p.first.c_str(), p.second));
		header.append(p.second + ";");
	}
	rapl_reader_ = all_readers_[0];
	powerlogger_ = std::make_unique<powerlogger>(logpath, header);
}

void powermeter_library_end() {
	rapl_reader_.reset();
	all_readers_.clear();
	power_targets_.clear();
	powerlogger_.reset();
}

float powermeter_library_current_powerdraw() {
	float cp = 0;
	if (!rapl_reader_){
		LOG_ERROR("No reader");
		return cp;
	}
	std::string logline;
	//Update all targets
	for(auto& p : all_readers_){
		auto c = p->current_powerdraw();
		logline.append(std::to_string(c) + ';');
		if(p == rapl_reader_) cp = c;
	}
	powerlogger_->log(logline);
	return cp;
}

float powermeter_library_total_energy_used() {
	if(!rapl_reader_) return 0;
	return rapl_reader_->total_energy_used();
}

unsigned int powermeter_library_powertarget_count(){
	return power_targets_.size();
}

void powermeter_library_get_powertarget(unsigned int pos, const char** path, const char** name){
	if(pos >= power_targets_.size()){
		path = nullptr;
		name = nullptr;
		return;
	}
	if(path) (*path) = power_targets_[pos].first.c_str();
	if(name) (*name) = power_targets_[pos].second.c_str();
}

void powermeter_library_set_powertarget_by_name(const char *name) {
	for(const auto &r : all_readers_){
		if(strcmp(r->name.c_str(), name) == 0){
			rapl_reader_ = r;
			LOG_INFO("Set new powertarget: " + name);
			return;
		}
	}
	LOG_ERROR("Powertarget not found " + name);
}

void powermeter_library_get_history(const char *name, float *buffer, int size) {
	if(!buffer || !name) return;
	for(const auto &r : all_readers_){
		if(strcmp(r->name.c_str(), name) == 0){
			r->fill_history(buffer, size);
			return;
		}
	}
	LOG_ERROR("Powertarget not found " + name);
}

void powermeter_library_set_logpath(const char *path) {
	std::string header;
	for(const auto& p : power_targets_) header.append(p.second + ";");
	powerlogger_ = std::make_unique<powerlogger>(path, header);
}

const char* powermeter_library_get_logpath(){
	return powerlogger_->path_.c_str();
}

}// } extern "C"