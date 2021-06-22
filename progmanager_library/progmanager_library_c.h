//
// Created by nano on 20.01.21.
//

#ifndef PROGMANAGER_LIBRARY_PROGMANAGER_LIBRARY_C_H
#define PROGMANAGER_LIBRARY_PROGMANAGER_LIBRARY_C_H


#ifdef __cplusplus
extern "C" {
#endif

	void progmanager_library_init();
	void progmanager_library_end();

	void progmanager_library_update();

	void progmanager_highest_util(int num, double* util, pid_t* pid);

	void progmanager_pid_info(pid_t pid, double* cpu_user, double* cpu_system, char* name, size_t name_len);

#ifdef __cplusplus
}
#endif

#endif //PROGMANAGER_LIBRARY_PROGMANAGER_LIBRARY_C_H
