
#include <stdio.h>
#include <stdlib.h>
#include "appliances.h"

Appliance* add_appliance(Appliance *arr, size_t *count, Appliance a) {
    Appliance *tmp = realloc(arr, (*count + 1) * sizeof(Appliance));
    if (!tmp) { perror("realloc"); return arr; }
    arr = tmp;
    arr[*count] = a;
    (*count)++;
    return arr;
}

Appliance* remove_appliance(Appliance *arr, size_t *count, size_t idx) {
    if (!arr || idx >= *count) return arr;
    for (size_t i = idx; i + 1 < *count; i++) arr[i] = arr[i + 1];
    (*count)--;
    if (*count == 0) { free(arr); return NULL; }
    Appliance *tmp = realloc(arr, (*count) * sizeof(Appliance));
    if (tmp) arr = tmp;
    return arr;
}

void toggle_appliance(Appliance *arr, size_t idx) {
    if (!arr) return;
    arr[idx].status = !arr[idx].status;
}

double daily_kwh(const Appliance *a) {
    if (!a) return 0.0;
    if (!a->status) return 0.0;
    return (a->wattage * a->hours_per_day) / 1000.0;
}

void print_report(Appliance *arr, size_t count, double tariff) {
    double total = 0.0;
    printf("\n===== ENERGY REPORT =====\n");
    printf("Idx\tName\tWatt\tHrs/day\tStatus\tDaily_kWh\n");
    for (size_t i = 0; i < count; i++) {
        double k = daily_kwh(&arr[i]);
        
        printf("%-4zu %-12s %-8.1f %-8.1f %-6s %.3f\n",
       i,
       arr[i].name,
       arr[i].wattage,
       arr[i].hours_per_day,
       arr[i].status ? "ON" : "OFF",
       k);

        total += k;
    }
    printf("\nTotal Daily kWh: %.3f\n", total);
    printf("Daily Cost: %.2f\n", total * tariff);
    printf("Weekly Cost: %.2f\n", total * tariff * 7);
    printf("Monthly Cost: %.2f\n", total * tariff * 30);
}
