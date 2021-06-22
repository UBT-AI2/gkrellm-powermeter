//
// Created by nano on 20.01.21.
//

#include <memory>
#include <string.h>
#include "progmanager_library.hpp"
#include "progmanager_library_c.h"

extern "C" {

static std::unique_ptr<progmanager_library> progmanager_lib_;

	void progmanager_library_init(){
		progmanager_lib_ = std::make_unique<progmanager_library>();
	}

	void progmanager_library_end(){
		progmanager_lib_.reset(nullptr);
	}

	void progmanager_library_update(){
		progmanager_lib_->update();
	}

	void progmanager_highest_util(pid_t num, double *util, pid_t *pid) {
		if(!util || !pid) return;
		//log error?
		auto highest = progmanager_lib_->highest_utilization(num);
		for(int i = 0; i < num; i++){
			util[i] = highest[i].first;
			pid[i] = highest[i].second;
		}
	}

	void progmanager_pid_info(pid_t pid, double *cpu_user, double *cpu_system, char *name, size_t name_len) {
		const auto& prog = progmanager_lib_->prog(pid);
		if(cpu_user) (*cpu_user) = prog->cpu_percent_user();
		if(cpu_system) (*cpu_system) = prog->cpu_percent_system();
		if(name) memcpy(name, prog->name().c_str(), name_len);
	}


}//extern "C"


