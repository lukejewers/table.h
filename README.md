# table.h

A single-header C library for creating and formatting tabular data in multiple output formats.

## Installation

Copy `table.h` into your project and include it:

```c
#include "table.h"
```

## Quick Start

```c
#include "table.h"

int main(void)
{
    // Create a 3-column table with default settings
    Table *table = table_create(
        .num_cols = 3,
        .output_stream = stdout
    );

    // Add rows
    table_row(table, "Name", "Age", "City");
    table_row(table, "Alice", "25", "New York");
    table_row(table, "Bob", "30", "London");

    // Print the table
    table_print(table);

    // Clean up
    table_free(table);
    return 0;
}
```

Output:
```
┌───────┬─────┬──────────┐
│ Name  │ Age │ City     │
├───────┼─────┼──────────┤
│ Alice │ 25  │ New York │
├───────┼─────┼──────────┤
│ Bob   │ 30  │ London   │
└───────┴─────┴──────────┘
```

## Configuration

Either use the convenience `table_create` macro:

```c
Table *table = table_create(
    .num_cols = 3,
    .output_format = FORMAT_SPACES,
    .border_style = BORDER_ASCII,
    .cell_padding = 2
);
```

or pass a `TableConfig` struct to `table_init`:
```c
TableConfig config = {
    .output_stream = stdout,
    .output_format = FORMAT_BORDERS,
    .border_style = BORDER_SINGLE,
    .even_col_spacing = false,
    .cell_padding = 1,
    .num_cols = 3,  // Required
};

Table *table = table_init(config);
```

### Configuration Options

| Field            | Type         | Default        | Description                    |
|------------------|--------------|----------------|--------------------------------|
| num_cols         | unsigned int | Required       | Number of columns in the table |
| output_stream    | FILE*        | stdout         | Where to print the table       |
| output_format    | OutputFormat | FORMAT_BORDERS | Table output format            |
| border_style     | BorderStyle  | BORDER_SINGLE  | Border drawing style           |
| even_col_spacing | bool         | false          | Make all columns equal width   |
| cell_padding     | unsigned int | 1              | Spaces inside each cell        |

### Output Formats

| Format           | Description              |
|------------------|--------------------------|
| `FORMAT_BORDERS` | Bordered table (default) |
| `FORMAT_CSV`     | Comma-separated values   |
| `FORMAT_SPACES`  | Space-aligned columns    |

### Border Styles

| Style           | Example |
|-----------------|---------|
| `BORDER_SINGLE` | `┌─┬─┐` |
| `BORDER_DOUBLE` | `╔═╦═╗` |
| `BORDER_ROUND`  | `╭─┬─╮` |
| `BORDER_ASCII`  | `+-+-+` |

## Building the Example

```console
make
./example
```
