#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ITEMS 200
#define TEXT_SIZE 80
#define DATA_FILE "items.txt"

typedef struct {
    int id;
    int type; /* 1 = lost, 2 = found */
    char name[TEXT_SIZE];
    char category[TEXT_SIZE];
    char color[TEXT_SIZE];
    char location[TEXT_SIZE];
    char date[TEXT_SIZE];
    int resolved;
} Item;

typedef struct {
    int index;
    int score;
} Match;

Item items[MAX_ITEMS];
int itemCount = 0;
int nextId = 1;

void readLine(const char *prompt, char *dest, size_t size) {
    size_t n;
    printf("%s", prompt);
    if (!fgets(dest, (int)size, stdin)) {
        dest[0] = '\0';
        return;
    }
    n = strlen(dest);
    if (n > 0 && dest[n - 1] == '\n') dest[n - 1] = '\0';
    else { int c; while ((c = getchar()) != '\n' && c != EOF) {} }
}

int readNumber(const char *prompt) {
    char line[40];
    int number; char extra;
    for (;;) {
        readLine(prompt, line, sizeof(line));
        if (sscanf(line, " %d %c", &number, &extra) == 1) return number;
        printf("Please enter a valid number.\n");
    }
}

int equalsIgnoreCase(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

void saveItems(void) {
    FILE *file = fopen(DATA_FILE, "w");
    int i;
    if (!file) { puts("Warning: Could not save data."); return; }
    for (i = 0; i < itemCount; i++) {
        Item *x = &items[i];
        fprintf(file, "%d|%d|%d|%s|%s|%s|%s|%s\n", x->id, x->type,
                x->resolved, x->name, x->category, x->color, x->location, x->date);
    }
    fclose(file);
}

void loadItems(void) {
    FILE *file = fopen(DATA_FILE, "r");
    char line[512];
    if (!file) return;
    while (itemCount < MAX_ITEMS && fgets(line, sizeof(line), file)) {
        Item x;
        char *part;
        int field = 0;
        char *fields[8];
        part = strtok(line, "|\n");
        while (part && field < 8) { fields[field++] = part; part = strtok(NULL, "|\n"); }
        if (field != 8) continue;
        x.id = atoi(fields[0]); x.type = atoi(fields[1]); x.resolved = atoi(fields[2]);
        if ((x.type != 1 && x.type != 2) || (x.resolved != 0 && x.resolved != 1)) continue;
        snprintf(x.name, sizeof(x.name), "%s", fields[3]);
        snprintf(x.category, sizeof(x.category), "%s", fields[4]);
        snprintf(x.color, sizeof(x.color), "%s", fields[5]);
        snprintf(x.location, sizeof(x.location), "%s", fields[6]);
        snprintf(x.date, sizeof(x.date), "%s", fields[7]);
        items[itemCount++] = x;
        if (x.id >= nextId) nextId = x.id + 1;
    }
    fclose(file);
}

void readField(const char *prompt, char *dest, size_t size) {
    for (;;) {
        readLine(prompt, dest, size);
        if (dest[0] && !strchr(dest, '|')) return;
        puts("Enter nonempty text without the | character.");
    }
}

void printItem(const Item *x) {
    printf("ID: %-4d | %-5s | %-9s | %s\n", x->id,
           x->type == 1 ? "LOST" : "FOUND", x->resolved ? "RESOLVED" : "OPEN", x->name);
    printf("     Category: %s | Color: %s | Location: %s | Date: %s\n",
           x->category, x->color, x->location, x->date);
}

void addItem(int type) {
    Item *x;
    if (itemCount == MAX_ITEMS) { puts("Storage is full."); return; }
    x = &items[itemCount];
    x->id = nextId++; x->type = type; x->resolved = 0;
    printf("\n--- Report a %s Item ---\n", type == 1 ? "LOST" : "FOUND");
    readField("Item name: ", x->name, sizeof(x->name));
    readField("Category (e.g. electronics): ", x->category, sizeof(x->category));
    readField("Color: ", x->color, sizeof(x->color));
    readField("Location: ", x->location, sizeof(x->location));
    readField("Date (YYYY-MM-DD): ", x->date, sizeof(x->date));
    itemCount++;
    saveItems();
    printf("Saved! Your report ID is %d.\n", x->id);
}

void viewItems(void) {
    int i;
    if (!itemCount) { puts("No reports yet."); return; }
    for (i = 0; i < itemCount; i++) printItem(&items[i]);
}

int matchScore(const Item *a, const Item *b) {
    int score = 0;
    if (equalsIgnoreCase(a->category, b->category)) score += 4;
    if (equalsIgnoreCase(a->color, b->color)) score += 3;
    if (equalsIgnoreCase(a->location, b->location)) score += 2;
    if (equalsIgnoreCase(a->name, b->name)) score += 1;
    return score;
}

void findMatches(void) {
    int id = readNumber("Enter your report ID: ");
    int i, j, count = 0;
    Match matches[MAX_ITEMS], temp;
    Item *target = NULL;
    for (i = 0; i < itemCount; i++) {
        if (items[i].id == id) { target = &items[i]; break; }
    }
    if (!target) { puts("Report not found."); return; }
    if (target->resolved) { puts("This report is already resolved."); return; }
    for (i = 0; i < itemCount; i++) {
        int score;
        if (items[i].type == target->type || items[i].resolved) continue;
        score = matchScore(target, &items[i]);
        if (score >= 4) { matches[count].index = i; matches[count].score = score; count++; }
    }
    /* Selection sort: largest scores first. */
    for (i = 0; i < count - 1; i++) {
        int best = i;
        for (j = i + 1; j < count; j++) {
            if (matches[j].score > matches[best].score) best = j;
        }
        temp = matches[i]; matches[i] = matches[best]; matches[best] = temp;
    }
    printf("\nPossible matches for report #%d (score out of 10):\n", id);
    if (!count) { puts("No likely matches found yet."); return; }
    for (i = 0; i < count; i++) {
        printf("\nMatch %d | Score: %d/10\n", i + 1, matches[i].score);
        printItem(&items[matches[i].index]);
    }
    puts("NOTE: A high score is only a suggestion, not proof of ownership.");
}

void resolveReport(void) {
    int id = readNumber("Report ID to mark resolved: ");
    int i;
    for (i = 0; i < itemCount; i++) {
        if (items[i].id == id) {
            if (items[i].resolved) { puts("Already resolved."); return; }
            items[i].resolved = 1;
            saveItems();
            puts("Report marked resolved.");
            return;
        }
    }
    puts("Report not found.");
}

int main(void) {
    int choice;
    loadItems();
    for (;;) {
        puts("\n========== CAMPUS LOST & FOUND ==========");
        puts("1. Report lost item");
        puts("2. Report found item");
        puts("3. View all reports");
        puts("4. Find possible matches");
        puts("5. Mark report as resolved");
        puts("6. Exit");
        choice = readNumber("Choose: ");
        switch (choice) {
            case 1: addItem(1); break;
            case 2: addItem(2); break;
            case 3: viewItems(); break;
            case 4: findMatches(); break;
            case 5: resolveReport(); break;
            case 6: puts("Goodbye!"); return 0;
            default: puts("Choose a number from 1 to 6.");
        }
    }
}
