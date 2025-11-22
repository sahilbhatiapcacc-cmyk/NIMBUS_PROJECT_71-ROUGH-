
#include <stdio.h>
#include <stdlib.h>
#include "appliances.h"

int main(void) {
    size_t count = 0;
    double tariff = 0.12;
    Appliance *arr = load_appliances("data/appliances.csv", &count);

    int choice;
    while (1) {
        printf("\n=== SMART HOME ENERGY DASHBOARD ===\n");
        printf("1) View Report\n2) Toggle Appliance\n3) Add Appliance\n4) Remove Appliance\n5) Change Tariff\n6) Save & Log & Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); printf("Invalid\n"); continue; }

        if (choice == 1) {
            print_report(arr, count, tariff);
        } else if (choice == 2) {
            if (count == 0) { printf("No appliances.\n"); continue; }
            size_t idx;
            printf("Enter index to toggle (0..%zu): ", count - 1);
            if (scanf("%zu", &idx) != 1 || idx >= count) { printf("Invalid\n"); while (getchar() != '\n'); continue; }
            toggle_appliance(arr, idx);
            printf("Toggled '%s' -> %s\n", arr[idx].name, arr[idx].status ? "ON" : "OFF");
        } else if (choice == 3) {
            Appliance a;
            printf("Name: "); scanf(" %[^\n]", a.name);
            printf("Wattage (W): "); scanf("%lf", &a.wattage);
            printf("Hours/day: "); scanf("%lf", &a.hours_per_day);
            a.status = 1;
            arr = add_appliance(arr, &count, a);
            printf("Added '%s'\n", a.name);
        } else if (choice == 4) {
            if (count == 0) { printf("No appliances.\n"); continue; }
            size_t idx;
            printf("Enter index to remove: ");
            if (scanf("%zu", &idx) != 1 || idx >= count) { printf("Invalid\n"); while (getchar() != '\n'); continue; }
            printf("Removing '%s'\n", arr[idx].name);
            arr = remove_appliance(arr, &count, idx);
        } else if (choice == 5) {
            printf("Current tariff: %.2f. Enter new tariff per kWh: ", tariff);
            if (scanf("%lf", &tariff) != 1) { printf("Invalid\n"); while (getchar() != '\n'); continue; }
            printf("Tariff set to %.2f\n", tariff);
        } else if (choice == 6) {
            if (save_appliances("data/appliances.csv", arr, count) != 0) printf("Save failed\n");
            log_consumption("data/consumption_log.txt", arr, count, tariff);
            printf("Saved & logged. Exiting.\n");
            break;
        } else {
            printf("Unknown choice\n");
        }
    }

    free(arr);
    return 0;
}


// in terminal:
// gcc main.c io.c ops.c -o dashboard.exe
// ./dashboard.exe
