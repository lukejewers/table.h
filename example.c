#include "table.h"

static void print_section(const char *title)
{
    printf("\n=== %s ===\n\n", title);
}

int main(void)
{
    print_section("Single Border");
    {
        TableConfig config = {
            .cell_padding = 1,
            .num_cols = 3,
            .output_stream = stdout,
            .output_format = FORMAT_BORDERS,
            .border_style = BORDER_SINGLE,
        };
        Table *table = table_init(config);
        table_row(table, "Name", "Age", "City");
        table_row(table, "Alice", "25", "New York");
        table_row(table, "Bob", "30", "London");
        table_print(table);
        free_table(table);
    }

    print_section("Double Border");
    {
        TableConfig config = {
            .cell_padding = 1,
            .num_cols = 3,
            .output_stream = stdout,
            .output_format = FORMAT_BORDERS,
            .border_style = BORDER_DOUBLE,
        };
        Table *table = table_init(config);
        table_row(table, "Product", "Price", "Stock");
        table_row(table, "Widget", "$19.99", "150");
        table_row(table, "Gadget", "$49.99", "75");
        table_print(table);
        free_table(table);
    }

    print_section("Rounded Border & Even Column Spacing");
    {
        TableConfig config = {
            .cell_padding = 1,
            .num_cols = 3,
            .output_stream = stdout,
            .output_format = FORMAT_BORDERS,
            .border_style = BORDER_ROUND,
            .even_col_spacing = true,
        };
        Table *table = table_init(config);
        table_row(table, "A", "BB", "CCC");
        table_row(table, "DDDD", "E", "FF");
        table_print(table);
        free_table(table);
    }

    print_section("ASCII Border");
    {
        TableConfig config = {
            .cell_padding = 1,
            .num_cols = 2,
            .output_stream = stdout,
            .output_format = FORMAT_BORDERS,
            .border_style = BORDER_ASCII,
        };
        Table *table = table_init(config);
        table_row(table, "Key", "Value");
        table_row(table, "debug", "true");
        table_row(table, "timeout", "30");
        table_print(table);
        free_table(table);
    }

    print_section("CSV Output");
    {
        TableConfig config = {
            .num_cols = 3,
            .output_stream = stdout,
            .output_format = FORMAT_CSV,
        };
        Table *table = table_init(config);
        table_row(table, "name", "email", "notes");
        table_row(table, "Alice", "alice@example.com", "First user");
        table_row(table, "Bob", "bob@example.com", "Says \"hello, world\"");
        table_print(table);
        free_table(table);
    }

    print_section("Space-Separated Output");
    {
        TableConfig config = {
            .num_cols = 3,
            .output_stream = stdout,
            .output_format = FORMAT_SPACES,
        };
        Table *table = table_init(config);
        table_row(table, "PID", "USER", "COMMAND");
        table_row(table, "1234", "root", "/sbin/init");
        table_row(table, "5678", "luke", "vim");
        table_print(table);
        free_table(table);
    }

    print_section("Using table_row_array()");
    {
        TableConfig config = {
            .cell_padding = 1,
            .num_cols = 2,
            .output_stream = stdout,
            .output_format = FORMAT_BORDERS,
            .border_style = BORDER_SINGLE,
        };
        Table *table = table_init(config);

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
