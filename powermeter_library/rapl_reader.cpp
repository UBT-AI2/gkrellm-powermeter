//
// Created by nano on 05.01.21.
//

#include "rapl_reader.h"

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <cstring>

#include "../progmanager_library/logger.hpp"

rapl_reader::rapl_reader(const char* path, std::string name)
	:
	time_point_begin_(std::chrono::high_resolution_clock::now()),
	time_point_end_(time_point_begin_),
	time_last_cycle_(1),
	energy_old_(0),
	rapl_path_(path),
	total_energy_used_(0),
	max_energy_(0),
	name(std::move(name)),
	history_(300, 0)
{
	//Read first entry as 0 power
	auto fd = open(rapl_path_.c_str(), O_RDONLY);
	if (fd < 0)
	{
		close(fd);
		LOG_ERROR("Powerfile not readable: " + rapl_path_)
	}
	else{
		char buffer[64];
		auto res = read(fd, buffer, 64);
		if(!res) memset(buffer, 0, 64);
		close(fd);
		energy_old_ = strtoul(buffer, nullptr, 0);
	}

	//Read max_energy_range_uj, for processing when the file flows over.
	std::string path_max_energy = rapl_path_.substr(
			0,rapl_path_.find_last_of('/')+1)+ "max_energy_range_uj";
	try{
		std::ifstream maxfile;
		maxfile.exceptions(std::ios_base::failbit);
		maxfile.open(path_max_energy);
		maxfile >> max_energy_;
		LOG_INFO(path_max_energy + ":\t" + std::to_string(max_energy_)
		+ ": " + std::to_string(max_energy_/(3600.f*1000000)) + "Wh")
	}
	catch (std::ios_base::failure &e) {
		LOG_ERROR("Unable to read " + path_max_energy + ": " + e.what())
	}
}


float rapl_reader::current_powerdraw() {
	time_point_begin_ = std::chrono::high_resolution_clock::now();
	time_last_cycle_ = std::chrono::duration_cast<std::chrono::milliseconds>(time_point_begin_ - time_point_end_);

	//open the file
	auto fd = open(rapl_path_.c_str(), O_RDONLY);
	if (fd < 0)
	{
		close(fd);
		LOG_ERROR("pnr " + rapl_path_)
		return -1;
	}

	//read and process the energy
	char buffer[64];
	auto res = read(fd, buffer, 64);
	close(fd);
	if(!res) {
		LOG_ERROR("Error reading energy_uj")
		return 0;
	}

	auto energy_curr = strtoul(buffer, nullptr, 0);
	long energy_used = long(energy_curr - energy_old_);

	//check for overflow
	if(energy_used < 0) {
		LOG_INFO(name + "Energy readout overflow")
		energy_used = max_energy_ == 0 ? energy_curr : max_energy_ + energy_used;
	}
	energy_old_ = energy_curr;

	//energy used in microjoules, time in milliseconds, so a factor of 1000 needed for Watts
	float power_used = float(energy_used) /
					   float(time_last_cycle_.count());
	power_used /= 1000;
	//Add the Watthour amount to the total
	//printf("Tlc: %ld\n",time_last_cycle_.count());
	total_energy_used_ += float(energy_used) / (3600.f*1000000);

	time_point_end_ = time_point_begin_;
	//LOG_INFO("returning " + std::to_string(power_used));
	history_[history_pos_++] = power_used;
	history_pos_ = history_pos_%int(history_.size());
	return power_used;
}

float rapl_reader::total_energy_used() const {
	return total_energy_used_;
}

std::vector<std::pair<std::string, std::string>> rapl_reader::power_targets(){
	//defined here for copy elision
	std::vector<std::pair<std::string, std::string>> entries;

	const char* PATH = "/sys/class/powercap/";
	DIR *dir = opendir(PATH);
	if (!dir){
		LOG_ERROR("Unable to open powertargets at " + PATH)
		closedir(dir);
		return entries;
	}
	struct dirent *entry = readdir(dir);

	entries.reserve(10);
	//Iterate over the directory to find all available powertargets
	while(entry != nullptr){
		if(entry->d_type == DT_LNK){
			//check if the directory has a ":" in its name, otherwise ignore it.
			if(std::string(entry->d_name).find(':') == std::string::npos){
				entry = readdir(dir);
				continue;
			}
			//otherwise read the name file inside the dir.
			std::string namepath = std::string(PATH) + std::string(entry->d_name) + "/name";
			std::string energypath = std::string(PATH) + std::string(entry->d_name) + "/energy_uj";
			try{
				std::ifstream namefile;
				namefile.open(namepath);
				std::string name;
				std::getline(namefile, name);

				//Add package number to name
				if((name == "core") || (name == "dram") || (name == "uncore")){
					std::string packagenamepath = std::string(PATH) + std::string(entry->d_name) + "/device/name";
					try{
						std::ifstream pnamefile;
						pnamefile.open(packagenamepath);
						std::string pname;
						std::getline(pnamefile, pname);
						name.append(pname.substr(pname.find_last_of('-')));
					}
					catch (const std::ios_base::failure &e) {
						LOG_ERROR("Error reading qualified name" + e.what())
					}
				}

				//Add entry to list of entries
				entries.emplace_back(energypath, name);
			} catch (const std::ios_base::failure &e) {
				LOG_ERROR("Error when reading name" + e.what())
			}
		}

		entry = readdir(dir);
	}
	closedir(dir);
	entries.shrink_to_fit();
	return entries;
}

void rapl_reader::fill_history(float *buffer, int size) {
	if(!buffer) return;
	int history_pos = history_pos_;
	for(int i = 0; i < std::min(size, int(history_.size())); i++){
		buffer[size - i] = history_[history_pos--];
		history_pos = (history_pos<0)*(int(history_.size()-1)) + (history_pos>=0)*history_pos;
	}
}
