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
    .output_format = TABLE_FMT_SPACE,
    .border_style = TABLE_BORDER_ASCII,
    .cell_padding = 2
);
```

or pass a `TableConfig` struct to `table_init`:
```c
TableConfig config = {
    .output_stream = stdout,
    .output_format = TABLE_FMT_BORDERS,
    .border_style = TABLE_BORDER_SINGLE,
    .even_col_spacing = false,
    .cell_padding = 1,
    .num_cols = 3,  // Required
};

Table *table = table_init(config);
```

### Configuration Options

| Field            | Type              | Default             | Description                      |
| ---------------- | ----------------- | ------------------- | -------------------------------- |
| num_cols         | unsigned int      | Required            | Number of columns in the table   |
| output_stream    | FILE*             | stdout              | Where to print the table         |
| output_format    | TableOutputFormat | TABLE_FMT_BORDERS   | Table output format              |
| border_style     | BorderStyle       | TABLE_BORDER_SINGLE | Border drawing style             |
| even_col_spacing | bool              | false               | Make all columns equal width     |
| cell_padding     | unsigned int      | 1                   | Spaces inside each cell          |
| alignment        | TableAlignment    | TABLE_ALIGN_LEFT    | Alignment of table cell contents |

### Output Formats

| Format              | Description               |
| ------------------- | ------------------------- |
| `TABLE_FMT_BORDERS` | Bordered table (default)  |
| `TABLE_FMT_CSV`     | Comma-separated values    |
| `TABLE_FMT_SPACE`  | Space-aligned columns     |
| `TABLE_FMT_PIPE`    | Pipe-delimited (Markdown) |

### Border Styles

| Style                 | Example |
| -------------------- -| ------- |
| `TABLE_BORDER_SINGLE` | `┌─┬─┐` |
| `TABLE_BORDER_DOUBLE` | `╔═╦═╗` |
| `TABLE_BORDER_ROUND`  | `╭─┬─╮` |
| `TABLE_BORDER_ASCII`  | `+-+-+` |

### Styling

You can use built-in macros to style individual cells. These use ANSI escape codes and work on most modern terminals.

```c
table_row(table,
    TABLE_BOLD("Header"),
    TABLE_RED("Critical"),
    TABLE_GREEN("Online")
);
```

## Building the Example

```console
make
./example
```
