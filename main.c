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

    table_row(table, "Name", "Age", "City");
    table_row(table, "Alice", "25", "New York");

    const char *data[] = {"Bob", "30", "London"};
    table_row_array(table, data);

    table_print(table);
    free_table(table);
    return 0;
}
