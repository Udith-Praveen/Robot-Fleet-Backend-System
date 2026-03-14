# Robot Fleet Backend System

![C](https://img.shields.io/badge/Language-C-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

## 🛠️ Project Brief: High-Efficiency Robot Fleet Manager (C)

### Project Overview
This project is a Production-Grade Robot Fleet Management System written in C. It is designed to simulate the backend infrastructure required to manage a fleet of autonomous warehouse robots. The system handles real-time data management, hardware-level status monitoring, and spatial collision avoidance.

---

### Core Technical Pillars

#### Dynamic Memory Management
- The system uses a dynamic array of robot structures.
- It utilizes `malloc`, `realloc`, and `free` to scale the fleet size at runtime.
- **Optimization:** Deletions are handled via `memmove` to shift memory in $O(1)$ allocation cycles, followed by a single hardware-level `realloc` to shrink the array, preventing memory thrashing.

#### Hardware-Aware Architecture
- **Struct Packing:** Uses `__attribute__((__packed__))` to eliminate compiler-injected padding. This ensures the binary data is consistent across different CPU architectures (e.g., x86 vs. ARM).
- **Bitwise Status Monitoring:** Uses a `uint8_t` bitmask to store and toggle hardware states (Engine, Lights, Alarms) in a single byte of memory.

#### Advanced Algorithms (Spatial Hashing)
- Replaced a standard $O(N^2)$ brute-force distance check with an Amortized $O(N)$ Spatial Hash Map.
- The warehouse is mathematically divided into a grid. Robots are mapped to "buckets" based on their $(X, Y)$ coordinates.
- **Collision Detection:** The system only calculates distances between robots sharing the same grid sector, drastically reducing CPU cycles as the fleet scales.

#### Binary Persistence (Persistence Layer)
- Implements a custom binary file I/O system (`fleet_data.bin`).
- On boot, the system reads the fleet count and raw struct data directly into RAM.
- On shutdown, it flushes the current state back to the disk, ensuring full persistence between sessions.

---

### Functional Features
- **Fleet Report:** Decodes bitwise hardware statuses into human-readable logs.
- **Modify Fleet:** Allows for adding/deleting robots and toggling specific hardware bits via XOR (`^`) operations.
- **Deployment Sorter:** Uses the standard library `qsort` with a custom comparator function to sort the fleet by battery level in $O(N \log N)$ time.
- **Emergency Protocol:** A safety routine that identifies low-battery robots, forces their engines/lights OFF, turns their Alarms ON, and re-sorts the fleet for priority charging.

---

### Security & Safety Implementations
- **Input Validation:** Uses a custom `get_safe_int` function to prevent buffer overflows and handle non-integer input gracefully.
- **Memory Safety:** Includes explicit checks for `realloc` failures and ensures all allocated memory (including the spatial grid) is freed before the program exits or returns from functions.
- **Data Sanitation:** Uses `strcspn` to strip newline characters from user-entered strings, ensuring the binary save file remains clean.

---

## 🚀 How to Compile and Run

1. Clone the repository:
   ```bash
   git clone https://github.com/Udith-Praveen/Robot-Fleet-Backend-System.git
   ```
2. Navigate to the directory:
   ```bash
   cd Robot-Fleet-Backend-System
   ```
3. Compile the multiple files using GCC:
   ```bash
   gcc -o fleet_manager main.c Robot_fleet_manager.c
   ```
4. Run the executable:
   ```bash
   ./fleet_manager
   ```
