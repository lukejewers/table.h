#include "table.h"

int main(void)
{
    printf("\n=== %s ===\n\n", "Single Border");
    {
        Table *table = table_create(.num_cols = 3);
        table_row(table, "Name", "Age", "City");
        table_row(table, "Alice", "25", "New York");
        table_row(table, "Bob", "30", "London");
        table_print(table);
        free_table(table);
    }

    printf("\n=== %s ===\n\n", "Double Border");
    {
        Table *table = table_create(.num_cols = 3, .border_style = BORDER_DOUBLE);
        table_row(table, "Product", "Price", "Stock");
        table_row(table, "Widget", "$19.99", "150");
        table_row(table, "Gadget", "$49.99", "75");
        table_print(table);
        free_table(table);
    }

    printf("\n=== %s ===\n\n", "Rounded Border & Even Column Spacing");
    {
        Table *table = table_create(.num_cols = 3, .border_style = BORDER_ROUND, .even_col_spacing = true);
        table_row(table, "A", "BB", "CCC");
        table_row(table, "DDDD", "E", "FF");
        table_print(table);
        free_table(table);
    }

    printf("\n=== %s ===\n\n", "ASCII Border");
    {
        Table *table = table_create(.num_cols = 2, .border_style = BORDER_ASCII);
        table_row(table, "Key", "Value");
        table_row(table, "debug", "true");
        table_row(table, "timeout", "30");
        table_print(table);
        free_table(table);
    }

    printf("\n=== %s ===\n\n", "CSV Output");
    {
        Table *table = table_create(.num_cols = 3, .output_format = FORMAT_CSV);
        table_row(table, "name", "email", "notes");
        table_row(table, "Alice", "alice@example.com", "First user");
        table_row(table, "Bob", "bob@example.com", "Says \"hello, world\"");
        table_print(table);
        free_table(table);
    }

    printf("\n=== %s ===\n\n", "Space-separated Output");
    {
        Table *table = table_create(.num_cols = 3, .output_format = FORMAT_SPACES);
        table_row(table, "PID", "USER", "COMMAND");
        table_row(table, "1234", "root", "/sbin/init");
        table_row(table, "5678", "luke", "vim");
        table_print(table);
        free_table(table);
    }

    printf("\n=== %s ===\n\n", "Using table_init()");
    {
        TableConfig config = {
            .cell_padding = 1,
            .num_cols = 3,
            .output_stream = stdout,
            .output_format = FORMAT_BORDERS,
            .border_style = BORDER_DOUBLE,
        };
        Table *table = table_init(config);
        table_row(table, "Name", "Age", "City");
        table_row(table, "Alice", "25", "New York");
        table_row(table, "Bob", "30", "London");
        table_print(table);
        free_table(table);
    }

    printf("\n=== %s ===\n\n", "Using table_row_array()");
    {
        Table *table = table_create(.num_cols = 2);
        const char *header[] = {"Option", "Description"};
        const char *row1[] = {"-h", "Show help"};
        const char *row2[] = {"-v", "Verbose output"};
        const char *row3[] = {"-o FILE", "Output to FILE"};
        table_row_array(table, header);
        table_row_array(table, row1);
        table_row_array(table, row2);
        table_row_array(table, row3);
        table_print(table);
        free_table(table);
    }

    return 0;
}
