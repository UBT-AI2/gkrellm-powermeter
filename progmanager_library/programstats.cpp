#include "programstats.hpp"


#include <cstdio>
#include <cstring>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include "logger.hpp"


programstats::programstats(pid_t pid)
	: pid_(pid)
{
	get_usage(pid_, &stat_current_);
	get_usage(pid_, &stat_old_);

	//read process name
	std::string filename = "/proc/";
	filename.append(std::to_string(pid_));
	filename.append("/comm");
	std::ifstream comm(filename, std::ifstream::in);
	char procname[256];
	comm.getline(procname, 256);

	name_ = procname;
}

programstats::~programstats()
= default;


double programstats::cpu_percent_user() const
{
	return cpu_user_;
}

double programstats::cpu_percent_system() const
{
	return cpu_system_;
}

void programstats::update()
{
	//get current status
	get_usage(pid_, &stat_current_);
	//calculate cpu_percentages and set variables
	cpu_percent(cpu_user_, cpu_system_);
	
	stat_old_ = stat_current_;
}

const std::string &programstats::name() const
{
	return name_;
}

pid_t programstats::pid() const
{
	return pid_;
}


/*
 * read /proc data into the passed struct pstat
 * returns 0 on success, -1 on error
*/
int programstats::get_usage(const pid_t pid, pstat* result)
{
	//convert  pid to string
	char pid_s[20];
	snprintf(pid_s, sizeof(pid_s), "%d", pid);
	char stat_filepath[30] = "/proc/"; strncat(stat_filepath, pid_s,
		sizeof(stat_filepath) - strlen(stat_filepath) - 1);
	strncat(stat_filepath, "/stat", sizeof(stat_filepath) -
		strlen(stat_filepath) - 1);

	FILE *fpstat = fopen(stat_filepath, "r");
	if (fpstat == nullptr) {
		LOG_ERROR(std::string("FOPEN ERROR /proc/stat/") + std::to_string(pid));
		return -1;
	}

	FILE *fstat = fopen("/proc/stat", "r");
	if (fstat == nullptr) {
		LOG_ERROR("FOPEN ERROR /proc/stat");
		fclose(fpstat);
		return -2;
	}

	//read values from /proc/pid/stat
	bzero(result, sizeof(struct pstat));
	long int rss;
	if (fscanf(fpstat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu"
		"%lu %ld %ld %*d %*d %*d %*d %*u %lu %ld",
		&result->utime_ticks, &result->stime_ticks,
		&result->cutime_ticks, &result->cstime_ticks, &result->vsize,
		&rss) == EOF) {
		fclose(fpstat);
		fclose(fstat);
		return -3;
	}
	fclose(fpstat);
	result->rss = rss * getpagesize();

	//read+calc cpu total time from /proc/stat
	long unsigned int cpu_time[10];
	bzero(cpu_time, sizeof(cpu_time));
	if (fscanf(fstat, "%*s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
		&cpu_time[0], &cpu_time[1], &cpu_time[2], &cpu_time[3],
		&cpu_time[4], &cpu_time[5], &cpu_time[6], &cpu_time[7],
		&cpu_time[8], &cpu_time[9]) == EOF) {
		fclose(fstat);
		return -4;
	}

	fclose(fstat);

	for (unsigned long i : cpu_time)
		result->cpu_total_time += i;

	return 0;
}

void programstats::calc_cpu_usage(const pstat * cur_usage, const pstat * last_usage, long unsigned int * ucpu_usage, long unsigned int * scpu_usage)
{
	*ucpu_usage = (cur_usage->utime_ticks + cur_usage->cutime_ticks) -
		(last_usage->utime_ticks + last_usage->cutime_ticks);

	*scpu_usage = (cur_usage->stime_ticks + cur_usage->cstime_ticks) -
		(last_usage->stime_ticks + last_usage->cstime_ticks);
}

void programstats::cpu_percent
(double& ucpu_usage, double& scpu_usage) const
{
	const long unsigned int total_time_diff = stat_current_.cpu_total_time -
		stat_old_.cpu_total_time;

	ucpu_usage = 100 * (((stat_current_.utime_ticks + stat_current_.cutime_ticks)
		- (stat_old_.utime_ticks + stat_old_.cutime_ticks))
		/ static_cast<double>(total_time_diff));

	scpu_usage = 100 * ((((stat_current_.stime_ticks + stat_current_.cstime_ticks)
		- (stat_old_.stime_ticks + stat_old_.cstime_ticks))) /
		static_cast<double>(total_time_diff));
}