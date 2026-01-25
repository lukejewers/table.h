# table_fmt.h

A single-header C library for creating and formatting tabular data in multiple output formats.

## Installation

Copy `table_fmt.h` into your project and include it:

```c
#include "table_fmt.h"
```

## Quick Start

```c
#include "table_fmt.h"

int main(void)
{
    // Create a 3-column table with default settings
    TableFmt *table = table_fmt_create(.num_cols = 3);

    // Add rows
    table_fmt_row(table, "Name", "Age", "City");
    table_fmt_row(table, "John", "18", "Paris");
    table_fmt_row(table, "Alice", "25", "New York");
    table_fmt_row(table, "Bob", "30", "London");

    // Print the table
    table_fmt_print(table);

    // Clean up
    table_fmt_free(table);
    return 0;
}
```

Output:
```
┌───────┬─────┬──────────┐
│ Name  │ Age │ City     │
├───────┼─────┼──────────┤
│ John  │ 18  │ Paris    │
│ Alice │ 25  │ New York │
│ Bob   │ 30  │ London   │
└───────┴─────┴──────────┘
```

## Configuration

Either use the convenience `table_fmt_create` macro:

```c
 *table = table_fmt_create(
    .border_type   = TABLE_FMT_BORDER_ASCII,
    .cell_padding  = 2
    .num_cols      = 3, // Required
    .output_format = TABLE_FMT_OUTPUT_SPACE,
);
```

or pass a `TableFmtConfig` struct to `table_fmt_init`:
```c
 config = {
    .border_type      = TABLE_FMT_BORDER_SINGLE,
    .cell_padding     = 1,
    .even_col_spacing = false,
    .num_cols         = 3, // Required
    .output_format    = TABLE_FMT_OUTPUT_BORDERS,
    .output_stream    = stdout,
};

 *table = table_fmt_init(config);
```

### Configuration Options

| Field            | Type               | Default                  | Description                               |
| ---------------- | ------------------ | ------------------------ | ----------------------------------------- |
| output_stream    | FILE*              | stdout                   | Where to print the table                  |
| header_style     | const char*        | NULL                     | Styling applied to the table header       |
| cell_padding     | size_t             | 1                        | Spaces inside each cell                   |
| num_cols         | size_t             | Required                 | Number of columns in the table            |
| output_format    | TableFmtOutput     | TABLE_FMT_OUTPUT_BORDERS | TableFmt output format                    |
| border_type      | TableFmtBorderType | TABLE_FMT_BORDER_SINGLE  | Border drawing type                       |
| alignment        | TableFmtAlignment  | TABLE_FMT_ALIGN_LEFT     | Alignment of table cell contents          |
| even_col_spacing | bool               | false                    | Make all columns equal width              |
| grid_lines       | bool               | false                    | Make every row have horizontal separators |

### Output Formats

| Format                     | Description               |
| -------------------------- | ------------------------- |
| `TABLE_FMT_OUTPUT_BORDERS` | Bordered table (default)  |
| `TABLE_FMT_OUTPUT_CSV`     | Comma-separated values    |
| `TABLE_FMT_OUTPUT_SPACE`   | Space-aligned columns     |
| `TABLE_FMT_OUTPUT_PIPE`    | Pipe-delimited (Markdown) |

### Border Types

| Type                      | Example   |
| ------------------------- | --------- |
| `TABLE_FMT_BORDER_SINGLE` | `┌──┬──┐` |
| `TABLE_FMT_BORDER_DOUBLE` | `╔══╦══╗` |
| `TABLE_FMT_BORDER_ROUND`  | `╭──┬──╮` |
| `TABLE_FMT_BORDER_ASCII`  | `+--+--+` |
| `TABLE_FMT_BORDER_PIPE`   | `\|  \|  \|` |

### Styling

The library provides ANSI escape sequences for terminal styling. These are designed to be used in two ways:

1. The Convenience Macro

Use TABLE_FMT_CELL_STYLE for a single-cell wrap. It automatically appends TABLE_FMT_RESET to prevent color "bleeding."

```c
table_fmt_row(table,
    TABLE_FMT_CELL_STYLE(TABLE_FMT_BOLD, "Status"),
    TABLE_FMT_CELL_STYLE(TABLE_FMT_GREEN, "ONLINE")
);
```

2. Manual Tags

For complex styling (like Bold + Red), you can stack tags manually. You must end the string with TABLE_FMT_RESET.

```c
table_fmt_row(table,
    TABLE_FMT_BOLD TABLE_FMT_RED "CRITICAL ERROR" TABLE_FMT_RESET,
    "Drive Failure"
);
```

## Building the Example

```console
make
./example
```
