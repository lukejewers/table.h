#include "table.h"

int main(void)
{
    TableConfig config = {
        .output_stream = stdout,
        .output_format = FORMAT_BORDERS,
        .border_style = BORDER_SINGLE,
        .even_col_spacing = false,
        .cell_padding = 1,
        .num_cols = 3,
    };
    Table *table = table_init(config);
    if (!table) return 1;

    table_row(table, "foo", "bar", "baz");
    table_row(table, "one", "three", "five");
    table_row(table, "a", "b", "c");

    table_print(table);

    free_table(table);
    return 0;
}
