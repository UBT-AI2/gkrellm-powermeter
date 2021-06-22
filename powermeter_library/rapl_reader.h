//
// Created by nano on 05.01.21.
//

#ifndef POWERMETER_LIBRARY_RAPL_READER_H
#define POWERMETER_LIBRARY_RAPL_READER_H

#include <chrono>
#include <string>
#include <vector>

class rapl_reader {
	std::chrono::time_point<std::chrono::high_resolution_clock> time_point_begin_ {};
	std::chrono::time_point<std::chrono::high_resolution_clock> time_point_end_ {};
	long unsigned int energy_old_;

	float total_energy_used_;
	unsigned long max_energy_;

	std::vector<float> history_;
	int history_pos_ = 0;

public:
	std::chrono::milliseconds time_last_cycle_;
	explicit rapl_reader(
			const char* path = "/sys/class/powercap/intel-rapl:0/energy_uj",
			std::string name = "");
	float current_powerdraw();
	float total_energy_used() const;
	//pairs of path_to_energy_uj and powertarget name
	static std::vector<std::pair<std::string, std::string>>	power_targets();
	void fill_history(float* buffer, int size);

	const std::string name;
	const std::string rapl_path_;
};


#endif //POWERMETER_LIBRARY_RAPL_READER_H
