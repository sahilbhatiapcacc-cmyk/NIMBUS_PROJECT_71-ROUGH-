
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "appliances.h"

/* Load CSV: name,wattage,hours,status */
Appliance* load_appliances(const char *filename, size_t *count) {
    FILE *f = fopen(filename, "r");
    *count = 0;
    if (!f) return NULL;

    Appliance temp;
    Appliance *arr = NULL;

    while (fscanf(f, "%49[^,],%lf,%lf,%d\n",
                  temp.name, &temp.wattage, &temp.hours_per_day, &temp.status) == 4) {
        Appliance *tmp = realloc(arr, (*count + 1) * sizeof(Appliance));
        if (!tmp) { perror("realloc"); free(arr); fclose(f); return NULL; }
        arr = tmp;
        arr[*count] = temp;
        (*count)++;
    }

    fclose(f);
    return arr;
}

/* Save CSV (overwrite) */
int save_appliances(const char *filename, Appliance *arr, size_t count) {
    FILE *f = fopen(filename, "w");
    if (!f) return -1;
    for (size_t i = 0; i < count; i++) {
        fprintf(f, "%s,%.2f,%.2f,%d\n",
                arr[i].name, arr[i].wattage, arr[i].hours_per_day, arr[i].status);
    }
    fclose(f);
    return 0;
}

/* Append log line with timestamp, total_kwh, daily_cost */
void log_consumption(const char *logfile, Appliance *arr, size_t count, double tariff) {
    FILE *f = fopen(logfile, "a");
    if (!f) return;
    double total = 0.0;
    for (size_t i = 0; i < count; i++) {
        total += (arr[i].status) ? (arr[i].wattage * arr[i].hours_per_day / 1000.0) : 0.0;
    }
    time_t now = time(NULL);
    struct tm *tt = localtime(&now);
    fprintf(f, "%04d-%02d-%02d %02d:%02d:%02d,%.3f,%.2f\n",
            tt->tm_year + 1900, tt->tm_mon + 1, tt->tm_mday,
            tt->tm_hour, tt->tm_min, tt->tm_sec,
            total, total * tariff);
    fclose(f);
}
