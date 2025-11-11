# Smart Medicine Reminder & Stock Tracker

A console-based C program to manage medicine stock, track expiry dates, and provide reminders for low-stock and soon-to-expire medicines. Ideal for pharmacies, clinics, or personal medicine tracking.

---

## Features

- **Medicine Management**
  - Add, update, delete, and display medicines.
  - Track medicine quantity, expiry date, price, and prescription status.
- **Search & Reminders**
  - Search medicines by exact name.
  - Filter medicines by expiry month/year.
  - Low-stock alerts.
  - Expiry reminders for a specific year.
- **Sorting**
  - Sort medicines by expiry date (soonest first).
  - Sort medicines by name alphabetically (A-Z).
- **Branch Management**
  - Merge medicines from a secondary branch.
  - Avoid duplicates when merging.
- **Bitwise & Demo Utilities**
  - Toggle availability flag using bitwise operations.
  - Swap integers without a temporary variable.
  - Demonstration of AND/OR/XOR operations.

---

## Getting Started

### Prerequisites
- GCC or any C compiler
- Terminal or Command Prompt

### Compilation
```bash
gcc -o medicine_tracker main.c
