#ifndef POWERMETER_LIBRARY_POWERMETER_LIBRARY_H
#define POWERMETER_LIBRARY_POWERMETER_LIBRARY_H

#ifdef __cplusplus
extern "C" {
#endif

void powermeter_library_init(const char* logpath);
void powermeter_library_end();

float powermeter_library_current_powerdraw();
float powermeter_library_total_energy_used();

unsigned int powermeter_library_powertarget_count();
void powermeter_library_get_powertarget(unsigned int pos, const char** path, const char** name);
void powermeter_library_set_powertarget_by_name(const char* name);
void powermeter_library_get_history(const char* name, float* buffer, int size);
void powermeter_library_set_logpath(const char* path);
const char* powermeter_library_get_logpath();

#ifdef __cplusplus
} // extern "C"
#endif

#endif //POWERMETER_LIBRARY_POWERMETER_LIBRARY_H
