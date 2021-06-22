#pragma once

#include <unistd.h>
#include <string>

class programstats
{
public:
	explicit programstats(pid_t pid);
	~programstats();

	double cpu_percent_user() const;
	double cpu_percent_system() const;

	void update();

	const std::string& name() const;
	pid_t pid() const;

private:
	struct pstat {
		long unsigned int utime_ticks;
		long int cutime_ticks;
		long unsigned int stime_ticks;
		long int cstime_ticks;
		long unsigned int vsize; // virtual memory size in bytes
		long unsigned int rss; //Resident  Set  Size in bytes

		long unsigned int cpu_total_time;
	};

	/* @author Fabian Holler
	 * Credit: https://github.com/fho/code_snippets/blob/master/c/getusage.c	*/
	static int get_usage(const pid_t pid, struct pstat* result);
	/* @author Fabian Holler
	 * Credit: https://github.com/fho/code_snippets/blob/master/c/getusage.c  */
	static void calc_cpu_usage(const struct pstat* cur_usage, const struct pstat* last_usage,
		 long unsigned int* ucpu_usage, long unsigned int* scpu_usage);
	/* @author Fabian Holler
	 * Credit: https://github.com/fho/code_snippets/blob/master/c/getusage.c  */
	void cpu_percent(double& ucpu_usage, double& scpu_usage) const;

	pstat stat_current_{};
	pstat stat_old_{};

	double cpu_user_ = 0;
	double cpu_system_ = 0;
	std::string name_;

	pid_t pid_;
};

