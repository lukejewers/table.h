# table.h

A lightweight single-header C library for creating and formatting tabular data in multiple output formats.

## Features

- Single Header: Just include table.h - no separate compilation needed.
- Multiple Output Formats: CSV and ASCII table formats
- Flexible Output: Print to stdout, stderr, or any file stream
- Easy to Use: Simple API for quick table creation
- Zero Dependencies: Only requires standard C library

## Installation

Simply copy table.h into your project and include it:

```c
#include "table.h"
```

Then compile your program normally:

```console
gcc -o myapp myapp.c
```

## Example Usage

```c
#include "table.h"

int main() {
    // Create a CSV table with 3 columns
    Table *table = table_init(stdout, FORMAT_CSV, 3);

    // Add rows of data
    table_row(table, "Name", "Age", "City");
    table_row(table, "Alice", "25", "New York");
    table_row(table, "Bob", "30", "London");

    // Print the table
    table_print(table);

    // Clean up
    free_table(table);
    return 0;
}
```
