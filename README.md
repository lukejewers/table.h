# table.h

A single-header C library for creating and formatting tabular data in multiple output formats.

## Features

- Single header: just include table.h - no separate compilation needed.
- Multiple output formats: CSV and ASCII table formats.
- Flexible output: print to stdout, stderr, or any file stream.
- Easy to use: simple API for quick table creation.
- Zero dependencies: only requires standard C library.

## Installation

Simply copy table.h into your project and include it:

```c
#include "table.h"
```

Then compile your program normally:

```console
gcc -o main main.c
```

## Example Usage

```c
#include "table.h"

int main() {
    // Define the table config
    TableConfig config = {
        .output_stream = stdout,
        .output_format = FORMAT_BORDERS,
        .border_style = BORDER_SINGLE,
        .even_col_spacing = false,
        .cell_padding = 1,
        .num_cols = 3,
    };
    Table *table = table_init(config);

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
