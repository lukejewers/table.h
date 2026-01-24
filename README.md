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
    .num_cols      = 3, // Required
    .output_format = TABLE_FMT_SPACE,
    .border_type   = TABLE_BORDER_ASCII,
    .cell_padding  = 2
);
```

or pass a `TableConfig` struct to `table_init`:
```c
TableConfig config = {
    .output_stream    = stdout,
    .output_format    = TABLE_FMT_BORDERS,
    .border_type      = TABLE_BORDER_SINGLE,
    .even_col_spacing = false,
    .cell_padding     = 1,
    .num_cols         = 3, // Required
};

Table *table = table_init(config);
```

### Configuration Options

| Field            | Type              | Default             | Description                      |
| ---------------- | ----------------- | ------------------- | -------------------------------- |
| output_stream    | FILE*             | stdout              | Where to print the table         |
| cell_padding     | size_t            | 1                   | Spaces inside each cell          |
| num_cols         | size_t            | Required            | Number of columns in the table   |
| output_format    | TableOutputFormat | TABLE_FMT_BORDERS   | Table output format              |
| border_type      | TableBorderStyle  | TABLE_BORDER_SINGLE | Border drawing type              |
| alignment        | TableAlignment    | TABLE_ALIGN_LEFT    | Alignment of table cell contents |
| even_col_spacing | bool              | false               | Make all columns equal width     |

### Output Formats

| Format              | Description               |
| ------------------- | ------------------------- |
| `TABLE_FMT_BORDERS` | Bordered table (default)  |
| `TABLE_FMT_CSV`     | Comma-separated values    |
| `TABLE_FMT_SPACE`   | Space-aligned columns     |
| `TABLE_FMT_PIPE`    | Pipe-delimited (Markdown) |

### Border Styles

| Style                 | Example |
| --------------------  | ------- |
| `TABLE_BORDER_SINGLE` | `┌─┬─┐` |
| `TABLE_BORDER_DOUBLE` | `╔═╦═╗` |
| `TABLE_BORDER_ROUND`  | `╭─┬─╮` |
| `TABLE_BORDER_ASCII`  | `+-+-+` |

### Styling

The library provides ANSI escape sequences for terminal styling. These are designed to be used in two ways:

1. The Convenience Macro

Use TABLE_CELL_STYLE for a single-cell wrap. It automatically appends TABLE_RESET to prevent color "bleeding."

```c
table_row(table,
    TABLE_CELL_STYLE(TABLE_BOLD, "Status"),
    TABLE_CELL_STYLE(TABLE_GREEN, "ONLINE")
);
```

2. Manual Tags

For complex styling (like Bold + Red), you can stack tags manually. You must end the string with TABLE_RESET.

```c
table_row(table,
    TABLE_BOLD TABLE_RED "CRITICAL ERROR" TABLE_RESET,
    "Drive Failure"
);
```

## Building the Example

```console
make
./example
```
