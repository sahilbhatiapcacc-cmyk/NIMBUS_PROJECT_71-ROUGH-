//          ---------MEMBER-1----------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char name[50];
    double wattage;
    double hours_per_day;
    int status; // 1 = ON, 0 = OFF
} Appliance;


Appliance* load_appliances(const char *filename, size_t *count);
int save_appliances(const char *filename, Appliance *arr, size_t count);
void log_consumption(const char *logfile, Appliance *arr, size_t count, double tariff);

Appliance* add_appliance(Appliance *arr, size_t *count, Appliance a);
Appliance* remove_appliance(Appliance *arr, size_t *count, size_t idx);
void toggle_appliance(Appliance *arr, size_t idx);

double daily_kwh(const Appliance *a);
void print_report(Appliance *arr, size_t count, double tariff);

Appliance* load_appliances(const char *filename, size_t *count) {
    FILE *f = fopen(filename, "r");
    *count = 0;
    if (!f) return NULL; // no file -> empty list

    Appliance temp;
    Appliance *arr = NULL;

    // CSV lines: name,wattage,hours,status
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


//          ---------MEMBER-2----------



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

void log_consumption(const char *logfile, Appliance *arr, size_t count, double tariff) {
    FILE *f = fopen(logfile, "a");
    if (!f) return;
    // write timestamp, total_kwh, daily_cost
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    double total = 0.0;
    for (size_t i = 0; i < count; i++) total += daily_kwh(&arr[i]);

    fprintf(f, "%04d-%02d-%02d %02d:%02d:%02d,%.3f,%.2f\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            total, total * tariff);
    fclose(f);
}

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

//          ---------MEMBER-3----------

void toggle_appliance(Appliance *arr, size_t idx) {
    if (!arr) return;
    arr[idx].status = !arr[idx].status;
}

double daily_kwh(const Appliance *a) {
    if (!a) return 0.0;
    if (a->status == 0) return 0.0;
    return (a->wattage * a->hours_per_day) / 1000.0;
}

void print_report(Appliance *arr, size_t count, double tariff) {
    double total = 0.0;
    printf("\n==== ENERGY REPORT ====\n");
    printf("Idx\tName\tWatt\tHrs/day\tStatus\tDaily_kWh\n");
    for (size_t i = 0; i < count; i++) {
        double k = daily_kwh(&arr[i]);
        printf("%zu\t%-10s\t%.1f\t%.1f\t%s\t%.3f\n",
               i, arr[i].name, arr[i].wattage, arr[i].hours_per_day,
               arr[i].status ? "ON" : "OFF", k);
        total += k;
    }
    printf("\nTotal Daily kWh: %.3f kWh\n", total);
    printf("Daily Cost: %.2f\n", total * tariff);
    printf("Weekly Cost: %.2f\n", total * tariff * 7);
    printf("Monthly Cost: %.2f\n", total * tariff * 30);
}



//          ---------MEMBER-4----------


int main(void) {
    size_t count = 0;
    double tariff = 0.12; // default currency per kWh
    Appliance *arr = load_appliances("data/appliances.csv", &count);

    int choice;
    while (1) {
        printf("\n=== SMART HOME ENERGY DASHBOARD ===\n");
        printf("1. View Report\n2. Toggle Appliance\n3. Add Appliance\n4. Remove Appliance\n5. Change Tariff\n6. Save & Log & Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) { // invalid input -> clear and continue
            while (getchar() != '\n'); printf("Invalid input\n"); continue;
        }
        if (choice == 1) {
            print_report(arr, count, tariff);
        } else if (choice == 2) {
            if (count == 0) { printf("No appliances.\n"); continue; }
            size_t idx;
            printf("Enter index to toggle (0..%zu): ", count - 1);
            if (scanf("%zu", &idx) != 1 || idx >= count) { printf("Invalid index\n"); while (getchar() != '\n'); continue; }
            toggle_appliance(arr, idx);
            printf("Toggled '%s' to %s\n", arr[idx].name, arr[idx].status ? "ON" : "OFF");
        } else if (choice == 3) {
            Appliance a;
            printf("Name: "); scanf(" %[^\n]", a.name);
            printf("Wattage (W): "); scanf("%lf", &a.wattage);
            printf("Hours per day: "); scanf("%lf", &a.hours_per_day);
            a.status = 1;
            arr = add_appliance(arr, &count, a);
            printf("Added '%s'\n", a.name);
        } else if (choice == 4) {
            if (count == 0) { printf("No appliances.\n"); continue; }
            size_t idx;
            printf("Enter index to remove: ");
            if (scanf("%zu", &idx) != 1 || idx >= count) { printf("Invalid index\n"); while (getchar() != '\n'); continue; }
            printf("Removing '%s'\n", arr[idx].name);
            arr = remove_appliance(arr, &count, idx);
        } else if (choice == 5) {
            printf("Current tariff: %.2f. Enter new tariff per kWh: ", tariff);
            if (scanf("%lf", &tariff) != 1) { printf("Invalid\n"); while (getchar() != '\n'); continue; }
            printf("Tariff set to %.2f\n", tariff);
        } else if (choice == 6) {
            if (save_appliances("data/appliances.csv", arr, count) != 0) printf("Failed to save appliances\n");
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
