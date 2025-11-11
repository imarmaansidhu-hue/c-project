#include <stdio.h>
#include <string.h>

#define MAX_MEDICINES 200
#define MAX_NAME_LEN 50
#define MAX_COMPANY_LEN 40
#define MAX_BRANCH 50

/* bit flags for medicine status (using an int for demonstration) */
#define FLAG_AVAILABLE  0x01  /* 1 => available */
#define FLAG_PRESCRIPTION 0x02 /* 1 => prescription required */

/* Medicine structure */
typedef struct {
    int id;                              /* unique id (simple int) */
    char name[MAX_NAME_LEN];             /* medicine name (no spaces advised) */
    char company[MAX_COMPANY_LEN];       /* manufacturer */
    int quantity;                        /* units in stock */
    int expiryMonth;                     /* 1..12 */
    int expiryYear;                      /* e.g., 2025 */
    int price;                           /* price per unit (integer rupees) */
    int flags;                           /* availability/prescription flags (bitwise) */
} Medicine;

/* Global arrays for main branch & secondary branch (for merge demo) */
static Medicine meds[MAX_MEDICINES];
static int medCount = 0;

/* Secondary branch storage for merge demo */
static Medicine branchMeds[MAX_BRANCH];
static int branchCount = 0;

/* Utility prototypes */
int readInt(const char prompt[], int min, int max);
void readString(const char prompt[], char dest[], int maxlen);
int findMedicineIndexById(int id);
int findMedicineIndexByName(const char name[]);
void swapMedicines(Medicine *a, Medicine *b);

/* Functional prototypes */
void populateSampleData(void);
void addMedicines(void);
void displayAllMedicines(void);
void displayMedicine(int idx);
void updateMedicineById(void);
void deleteMedicineById(void);
void searchByName(void);
void searchByExpiry(void);
void lowStockReminder(void);
void expiryReminderForYear(int year);
void sortByExpirySoonest(void);
void sortByNameAsc(void);
void toggleAvailabilityById(void);
void mergeBranchIntoMain(void);
void branchAddSample(void);

/* Small demo utilities */
void swapWithoutThirdVarDemo(void);
void bitwiseDemo(void);

/* Menu */
void mainMenu(void);

/* Implementation */

/* Read integer with prompt and bounds; validates input */
int readInt(const char prompt[], int min, int max) {
    int x;
    int rc;
    while (1) {
        printf("%s", prompt);
        rc = scanf("%d", &x);
        if (rc != 1) {
            while (getchar() != '\n'); /* flush invalid input */
            printf("Invalid input. Please enter an integer value.\n");
            continue;
        }
        while (getchar() != '\n'); /* flush newline */
        if (min <= max) {
            if (x < min) {
                printf("Value too small. Using %d.\n", min);
                x = min;
            }
            if (x > max) {
                printf("Value too large. Using %d.\n", max);
                x = max;
            }
        }
        return x;
    }
}

/* Read a single-token string (no spaces) safely */
void readString(const char prompt[], char dest[], int maxlen) {
    printf("%s", prompt);
    /* width specifier prevents overflow; leave space for null */
    if (maxlen > 1) {
        char fmt[16];
        /* build format like "%49s" */
        sprintf(fmt, "%%%ds", maxlen - 1);
        scanf(fmt, dest);
    } else {
        scanf("%s", dest);
    }
    while (getchar() != '\n'); /* flush rest of line */
}

/* Find medicine index by ID; -1 if not found */
int findMedicineIndexById(int id) {
    int i;
    for (i = 0; i < medCount; ++i) {
        if (meds[i].id == id) return i;
    }
    return -1;
}

/* Find medicine index by exact name; -1 if not found */
int findMedicineIndexByName(const char name[]) {
    int i;
    for (i = 0; i < medCount; ++i) {
        if (strcmp(meds[i].name, name) == 0) return i;
    }
    return -1;
}

/* Swap two Medicine structs (used in sorting) */
void swapMedicines(Medicine *a, Medicine *b) {
    Medicine tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Add a few sample medicines to speed testing */
void populateSampleData(void) {
    if (medCount > 0) {
        printf("Sample data already present; not adding.\n");
        return;
    }
    medCount = 5;
    meds[0].id = 101; strcpy(meds[0].name, "Paracetamol"); strcpy(meds[0].company, "HealCo");
    meds[0].quantity = 120; meds[0].expiryMonth = 11; meds[0].expiryYear = 2025; meds[0].price = 5;
    meds[0].flags = FLAG_AVAILABLE; /* available */

    meds[1].id = 102; strcpy(meds[1].name, "Ibuprofen"); strcpy(meds[1].company, "CureLabs");
    meds[1].quantity = 60; meds[1].expiryMonth = 4; meds[1].expiryYear = 2024; meds[1].price = 8;
    meds[1].flags = FLAG_AVAILABLE | FLAG_PRESCRIPTION;

    meds[2].id = 103; strcpy(meds[2].name, "Cetirizine"); strcpy(meds[2].company, "Allergix");
    meds[2].quantity = 10; meds[2].expiryMonth = 2; meds[2].expiryYear = 2024; meds[2].price = 3;
    meds[2].flags = FLAG_AVAILABLE;

    meds[3].id = 104; strcpy(meds[3].name, "Amoxicillin"); strcpy(meds[3].company, "BioPharm");
    meds[3].quantity = 0; meds[3].expiryMonth = 8; meds[3].expiryYear = 2023; meds[3].price = 12;
    meds[3].flags = 0; /* not available */

    meds[4].id = 105; strcpy(meds[4].name, "VitaminC"); strcpy(meds[4].company, "NutriPlus");
    meds[4].quantity = 200; meds[4].expiryMonth = 6; meds[4].expiryYear = 2026; meds[4].price = 2;
    meds[4].flags = FLAG_AVAILABLE;

    printf("Added 5 sample medicines (IDs: 101..105).\n");
}

/* Add medicines interactively */
void addMedicines(void) {
    int n = readInt("How many medicines to add? ", 1, MAX_MEDICINES - medCount);
    int i;
    for (i = 0; i < n; ++i) {
        if (medCount >= MAX_MEDICINES) {
            printf("Medicine database full; cannot add more.\n");
            break;
        }
        Medicine m;
        m.id = readInt("Enter medicine id (integer): ", 1, 1000000);
        if (findMedicineIndexById(m.id) != -1) {
            printf("ID already exists. Skipping this entry.\n");
            i--; /* try again for this slot */
            continue;
        }
        readString("Enter medicine name (no spaces): ", m.name, MAX_NAME_LEN);
        readString("Enter company name (no spaces): ", m.company, MAX_COMPANY_LEN);
        m.quantity = readInt("Enter quantity in stock: ", 0, 1000000);
        m.expiryMonth = readInt("Enter expiry month (1-12): ", 1, 12);
        m.expiryYear = readInt("Enter expiry year (e.g., 2025): ", 2020, 9999);
        m.price = readInt("Enter price per unit (integer): ", 0, 1000000);
        printf("Is this prescription-only? 1=Yes 0=No: ");
        { int pres; scanf("%d", &pres); while (getchar() != '\n'); if (pres) m.flags = FLAG_PRESCRIPTION; else m.flags = 0; }
        if (m.quantity > 0) m.flags |= FLAG_AVAILABLE;
        meds[medCount++] = m;
        printf("Medicine added. Current total medicines: %d\n", medCount);
    }
}

/* Display a single medicine (by index) */
void displayMedicine(int idx) {
    if (idx < 0 || idx >= medCount) {
        printf("Invalid index.\n");
        return;
    }
    Medicine *m = &meds[idx];
    printf("ID: %d\n", m->id);
    printf("Name: %s\n", m->name);
    printf("Company: %s\n", m->company);
    printf("Quantity: %d\n", m->quantity);
    printf("Expiry: %02d/%04d\n", m->expiryMonth, m->expiryYear);
    printf("Price: %d\n", m->price);
    printf("Flags: ");
    if (m->flags & FLAG_AVAILABLE) printf("Available ");
    else printf("NotAvailable ");
    if (m->flags & FLAG_PRESCRIPTION) printf("| Prescription");
    printf("\n");
}

/* Display all medicines in tabular form */
void displayAllMedicines(void) {
    int i;
    if (medCount == 0) {
        printf("No medicines in database.\n");
        return;
    }
    printf("No  ID     Name                 Company          Qty  Expiry    Price  Avl Pres\n");
    for (i = 0; i < medCount; ++i) {
        Medicine *m = &meds[i];
        printf("%-3d %-6d %-20s %-15s %-4d  %02d/%04d  %-5d  %c   %c\n",
               i+1, m->id, m->name, m->company, m->quantity,
               m->expiryMonth, m->expiryYear, m->price,
               (m->flags & FLAG_AVAILABLE) ? 'Y' : 'N',
               (m->flags & FLAG_PRESCRIPTION) ? 'Y' : 'N');
    }
}

/* Update medicine by id */
void updateMedicineById(void) {
    if (medCount == 0) {
        printf("No medicines to update.\n");
        return;
    }
    int id = readInt("Enter medicine id to update: ", 1, 1000000);
    int idx = findMedicineIndexById(id);
    if (idx == -1) {
        printf("Medicine with id %d not found.\n", id);
        return;
    }
    Medicine *m = &meds[idx];
    printf("Updating medicine ID %d (%s)\n", m->id, m->name);
    int choice = readInt("Which field? 1:Name 2:Company 3:Quantity 4:Expiry 5:Price 6:Toggle Prescription 7:Back : ", 1, 7);
    if (choice == 1) {
        readString("Enter new name: ", m->name, MAX_NAME_LEN);
    } else if (choice == 2) {
        readString("Enter new company: ", m->company, MAX_COMPANY_LEN);
    } else if (choice == 3) {
        int q = readInt("Enter new quantity: ", 0, 1000000);
        m->quantity = q;
        if (q > 0) m->flags |= FLAG_AVAILABLE; else m->flags &= ~FLAG_AVAILABLE;
    } else if (choice == 4) {
        m->expiryMonth = readInt("Enter new expiry month (1-12): ", 1, 12);
        m->expiryYear = readInt("Enter new expiry year: ", 2020, 9999);
    } else if (choice == 5) {
        m->price = readInt("Enter new price: ", 0, 1000000);
    } else if (choice == 6) {
        m->flags ^= FLAG_PRESCRIPTION; /* toggle */
        printf("Prescription flag toggled.\n");
    } else {
        printf("Back to menu.\n");
    }
    printf("Update complete for id %d.\n", m->id);
}

/* Delete medicine by id */
void deleteMedicineById(void) {
    if (medCount == 0) {
        printf("No medicines to delete.\n");
        return;
    }
    int id = readInt("Enter medicine id to delete: ", 1, 1000000);
    int idx = findMedicineIndexById(id);
    if (idx == -1) {
        printf("Medicine with id %d not found.\n", id);
        return;
    }
    int i;
    for (i = idx; i < medCount - 1; ++i) meds[i] = meds[i+1];
    medCount--;
    printf("Medicine id %d deleted.\n", id);
}

/* Search by exact name */
void searchByName(void) {
    if (medCount == 0) { printf("Database empty.\n"); return; }
    char q[MAX_NAME_LEN];
    readString("Enter exact name to search: ", q, MAX_NAME_LEN);
    int idx = findMedicineIndexByName(q);
    if (idx == -1) { printf("Medicine '%s' not found.\n", q); return; }
    displayMedicine(idx);
}

/* Search by expiry month/year or show those expiring in given month/year */
void searchByExpiry(void) {
    if (medCount == 0) { printf("Database empty.\n"); return; }
    int month = readInt("Enter expiry month (1-12) or 0 to skip month: ", 0, 12);
    int year = readInt("Enter expiry year (e.g., 2024) or 0 to skip year: ", 0, 9999);
    int found = 0;
    int i;
    for (i = 0; i < medCount; ++i) {
        int ok = 1;
        if (month != 0 && meds[i].expiryMonth != month) ok = 0;
        if (year != 0 && meds[i].expiryYear != year) ok = 0;
        if (ok) {
            displayMedicine(i);
            found = 1;
        }
    }
    if (!found) printf("No medicines match the expiry filter.\n");
}

/* Low-stock reminder: list medicines with quantity <= threshold */
void lowStockReminder(void) {
    if (medCount == 0) { printf("Database empty.\n"); return; }
    int thresh = readInt("Enter low-stock threshold (e.g., 20): ", 0, 1000000);
    int found = 0;
    int i;
    for (i = 0; i < medCount; ++i) {
        if (meds[i].quantity <= thresh) {
            printf("Low stock: ID %d Name %s Qty %d\n", meds[i].id, meds[i].name, meds[i].quantity);
            found = 1;
        }
    }
    if (!found) printf("No medicines with quantity <= %d\n", thresh);
}

/* Expiry reminder for a particular year (show those expiring that year or before) */
void expiryReminderForYear(int year) {
    if (medCount == 0) { printf("Database empty.\n"); return; }
    int found = 0;
    int i;
    for (i = 0; i < medCount; ++i) {
        if (meds[i].expiryYear <= year) {
            printf("Expiring on/before %d: ID %d Name %s Expiry %02d/%04d\n",
                   year, meds[i].id, meds[i].name, meds[i].expiryMonth, meds[i].expiryYear);
            found = 1;
        }
    }
    if (!found) printf("No medicines expiring on/before %d\n", year);
}

/* Sort by expiry soonest (ascending year, then month) */
void sortByExpirySoonest(void) {
    if (medCount < 2) { printf("Not enough medicines to sort.\n"); return; }
    int i, j;
    for (i = 0; i < medCount - 1; ++i) {
        for (j = i + 1; j < medCount; ++j) {
            if (meds[j].expiryYear < meds[i].expiryYear ||
                (meds[j].expiryYear == meds[i].expiryYear && meds[j].expiryMonth < meds[i].expiryMonth))
            {
                swapMedicines(&meds[i], &meds[j]);
            }
        }
    }
    printf("Sorted by expiry date (soonest first).\n");
}

/* Sort by name ascending */
void sortByNameAsc(void) {
    if (medCount < 2) { printf("Not enough medicines to sort.\n"); return; }
    int i, j;
    for (i = 0; i < medCount - 1; ++i) {
        for (j = i + 1; j < medCount; ++j) {
            if (strcmp(meds[i].name, meds[j].name) > 0) swapMedicines(&meds[i], &meds[j]);
        }
    }
    printf("Sorted by name (A-Z).\n");
}

/* Toggle availability flag for a medicine (demo of bitwise usage) */
void toggleAvailabilityById(void) {
    if (medCount == 0) { printf("No medicines.\n"); return; }
    int id = readInt("Enter medicine id to toggle availability: ", 1, 1000000);
    int idx = findMedicineIndexById(id);
    if (idx == -1) { printf("Not found.\n"); return; }
    meds[idx].flags ^= FLAG_AVAILABLE; /* toggle bit */
    printf("Toggled availability for id %d. Now %s\n", id, (meds[idx].flags & FLAG_AVAILABLE) ? "Available" : "Not Available");
}

/* Merge branch list into main meds[] excluding duplicates by name */
void mergeBranchIntoMain(void) {
    if (branchCount == 0) { printf("Branch list empty. Use branch sample add or manual entry first.\n"); return; }
    int i, j;
    for (i = 0; i < branchCount; ++i) {
        int dup = 0;
        for (j = 0; j < medCount; ++j) {
            if (strcmp(branchMeds[i].name, meds[j].name) == 0) { dup = 1; break; }
        }
        if (dup) {
            printf("Duplicate '%s' skipped.\n", branchMeds[i].name);
            continue;
        }
        if (medCount >= MAX_MEDICINES) {
            printf("Main DB full; cannot merge more.\n");
            break;
        }
        meds[medCount++] = branchMeds[i];
        printf("Merged '%s' into main DB.\n", branchMeds[i].name);
    }
    printf("Merge complete. Main med count: %d\n", medCount);
}

/* Add sample data to branch list (for merge demo) */
void branchAddSample(void) {
    branchCount = 3;
    branchMeds[0].id = 201; strcpy(branchMeds[0].name, "Dolo650"); strcpy(branchMeds[0].company, "MediCorp");
    branchMeds[0].quantity = 50; branchMeds[0].expiryMonth = 12; branchMeds[0].expiryYear = 2025; branchMeds[0].price = 6; branchMeds[0].flags = FLAG_AVAILABLE;

    branchMeds[1].id = 202; strcpy(branchMeds[1].name, "Paracetamol"); strcpy(branchMeds[1].company, "HealCo");
    branchMeds[1].quantity = 30; branchMeds[1].expiryMonth = 10; branchMeds[1].expiryYear = 2024; branchMeds[1].price = 5; branchMeds[1].flags = FLAG_AVAILABLE;

    branchMeds[2].id = 203; strcpy(branchMeds[2].name, "Zincovit"); strcpy(branchMeds[2].company, "NutraLife");
    branchMeds[2].quantity = 90; branchMeds[2].expiryMonth = 6; branchMeds[2].expiryYear = 2026; branchMeds[2].price = 15; branchMeds[2].flags = FLAG_AVAILABLE;

    printf("Branch sample data added (3 items).\n");
}

/* Small demo: swap two integers without third variable */
void swapWithoutThirdVarDemo(void) {
    int a = readInt("Enter a: ", -1000000, 1000000);
    int b = readInt("Enter b: ", -1000000, 1000000);
    printf("Before swap: a=%d b=%d\n", a, b);
    a = a + b;
    b = a - b;
    a = a - b;
    printf("After swap: a=%d b=%d\n", a, b);
}

/* Small bitwise demo (AND/OR/XOR + smallest) */
void bitwiseDemo(void) {
    int x = readInt("Enter first integer: ", -1000000, 1000000);
    int y = readInt("Enter second integer: ", -1000000, 1000000);
    int andv = x & y;
    int orv = x | y;
    int xorv = x ^ y;
    printf("AND=%d OR=%d XOR=%d\n", andv, orv, xorv);
    int smallest = andv;
    if (orv < smallest) smallest = orv;
    if (xorv < smallest) smallest = xorv;
    printf("Smallest among AND/OR/XOR = %d\n", smallest);
}

/* Main menu */
void mainMenu(void) {
    int choice;
    printf("Smart Medicine Reminder & Stock Tracker\n");
    printf("First, you may populate sample data (option 0) for quick testing.\n");
    do {
        printf("\nMain Menu\n");
        printf("0. Populate sample data (quick test)\n");
        printf("1. Add medicines\n");
        printf("2. Display all medicines\n");
        printf("3. Update medicine by id\n");
        printf("4. Delete medicine by id\n");
        printf("5. Search by name\n");
        printf("6. Search by expiry month/year\n");
        printf("7. Low-stock reminder\n");
        printf("8. Expiry reminder (by year)\n");
        printf("9. Sort by expiry date (soonest)\n");
        printf("10. Sort by name (A-Z)\n");
        printf("11. Toggle availability (bitwise demo)\n");
        printf("12. Merge branch data into main (branch sample available)\n");
        printf("13. Add branch sample data (for merge demo)\n");
        printf("14. Swap without third variable demo\n");
        printf("15. Bitwise AND/OR/XOR demo\n");
        printf("16. Exit\n");
        choice = readInt("Enter choice: ", 0, 16);
        switch (choice) {
            case 0: populateSampleData(); break;
            case 1: addMedicines(); break;
            case 2: displayAllMedicines(); break;
            case 3: updateMedicineById(); break;
            case 4: deleteMedicineById(); break;
            case 5: searchByName(); break;
            case 6: searchByExpiry(); break;
            case 7: lowStockReminder(); break;
            case 8: { int y = readInt("Enter year to check expiry on/before: ", 2020, 9999); expiryReminderForYear(y); break; }
            case 9: sortByExpirySoonest(); break;
            case 10: sortByNameAsc(); break;
            case 11: toggleAvailabilityById(); break;
            case 12: mergeBranchIntoMain(); break;
            case 13: branchAddSample(); break;
            case 14: swapWithoutThirdVarDemo(); break;
            case 15: bitwiseDemo(); break;
            case 16: printf("Exiting program.\n"); break;
            default: printf("Invalid choice.\n"); break;
        }
    } while (choice != 16);
}

/* Program entry */
int main(void) {
    mainMenu();
    return 0;
}
