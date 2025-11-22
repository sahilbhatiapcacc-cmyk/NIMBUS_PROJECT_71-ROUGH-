
#ifndef APPLIANCES_H
#define APPLIANCES_H

#include <stddef.h>

typedef struct {
    char name[50];
    double wattage;
    double hours_per_day;
    int status;
} Appliance;

/* IO functions (implemented in io.c) */
Appliance* load_appliances(const char *filename, size_t *count);
int save_appliances(const char *filename, Appliance *arr, size_t count);
void log_consumption(const char *logfile, Appliance *arr, size_t count, double tariff);

/* Operations (implemented in ops.c) */
Appliance* add_appliance(Appliance *arr, size_t *count, Appliance a);
Appliance* remove_appliance(Appliance *arr, size_t *count, size_t idx);
void toggle_appliance(Appliance *arr, size_t idx);
double daily_kwh(const Appliance *a);
void print_report(Appliance *arr, size_t count, double tariff);

#endif

