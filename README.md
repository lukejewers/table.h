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
    TableConfig config = {
        .cell_padding = 1,
        .output_stream = stdout,
    };
    Table *table = table_init(3, config);

    table_row(table, "Name", "Age", "City");
    table_row(table, "Alice", "25", "New York");
    table_row(table, "Bob", "30", "London");

    table_print(table);
    free_table(table);
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

```c
TableConfig config = {
    .output_stream = stdout,
    .output_format = FORMAT_BORDERS,
    .border_style = BORDER_SINGLE,
    .cell_padding = 1,
    .even_col_spacing = false,
};
Table *table = table_init(3, config);  // 3 columns
```

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
