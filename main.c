#include "table.h"

int main(void)
{
    unsigned int num_cols = 3;
    Table *table = table_init(stdout, FORMAT_BORDERS, BORDER_ROUND, num_cols);
    if (!table) return 1;

    table_row(table, "foo", "bar", "baz");
    table_row(table, "one", "three", "five");
    table_row(table, "a", "b", "c");

    table_print(table);

    free_table(table);
    return 0;
}
