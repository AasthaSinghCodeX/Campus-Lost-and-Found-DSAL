# Campus Lost & Found Matching System

### Data Structures and Algorithms Mini Project

**Course:** Data Structures and Algorithms Lab (DSAL)  
**Programming Language:** C  
**Institution:** NMIMS, Mukesh Patel School of Technology Management & Engineering  
**Program:** MBA Tech Computer Engineering

**Submitted By:**
- Aastha Singh (N118)
- Riya Surve (N128)

**Faculty Guide:** Deepti Reddy

## Project Description

The Campus Lost & Found Matching System is a C-based application designed to help students report and locate lost belongings within a college campus.

The system stores details of lost and found items and compares them to identify possible matches based on item name, category, color, and location.

## Features

1. Register lost items.
2. Register found items.
3. Display all reported items.
4. Search for possible matches.
5. Rank matching items using a scoring system.
6. Mark reports as resolved.
7. Save and retrieve records using file handling.

## Data Structures and Algorithms Used

- **Structures:** Store item details such as name, category, color, location, date, and report ID.
- **Arrays:** Maintain a collection of lost and found reports.
- **Linear Search:** Find reports using their unique IDs.
- **Selection Sort:** Arrange possible matches according to their scores.
- **File Handling:** Store records for future use.

## Matching Algorithm

The system compares lost and found items using the following scoring criteria:

| Criteria | Score |
|---|---|
| Same category | 4 |
| Same color | 3 |
| Same location | 2 |
| Same item name | 1 |
| **Maximum Score** | **10** |

Items with higher scores are displayed first as possible matches.

## How to Run

1. Download the `main.c` file.
2. Open a terminal in the folder containing the file.
3. Compile the program using GCC:

   `gcc -std=c11 main.c -o lostfound`

4. Run the program:

   `./lostfound`

5. Choose an option from the menu to use the system.

## Conclusion

The project demonstrates the use of basic data structures and algorithms in a practical application. It uses structured records, searching, and sorting to organize reported items and identify possible matches within a campus.
