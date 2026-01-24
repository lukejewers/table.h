#include "table.h"

int main(void)
{
    printf("\n=== %s ===\n", "Single Border (Default)");
    {
        Table *table = table_create(.num_cols = 3);
        table_row(table, "Planet", "Diameter", "Type");
        table_row(table, "Jupiter", "139,820 km", "Gas Giant");
        table_row(table, "Mars", "6,779 km", "Terrestrial");
        table_row(table, "Neptune", "49,244 km", "Ice Giant");
        table_print(table);
        table_free(table);
    }

    printf("\n=== %s ===\n", "Double Border & Rich Styling");
    {
        Table *table = table_create(.num_cols = 3, .border_style = TABLE_BORDER_DOUBLE);
        table_row(table, TABLE_BOLD("Server"), TABLE_BOLD("Uptime"), TABLE_BOLD("Status"));
        table_row(table, "US-East-1", "142 days", TABLE_GREEN("OPERATIONAL"));
        table_row(table, "EU-Central-1", "12 hours", TABLE_YELLOW("DEGRADED"));
        table_row(table, "AP-South-1", "0 days", TABLE_RED("OFFLINE"));
        table_print(table);
        table_free(table);
    }

    printf("\n=== %s ===\n", "Rounded Border & Even Column Spacing");
    {
        Table *table = table_create(
            .num_cols = 4,
            .border_style = TABLE_BORDER_ROUND,
            .even_col_spacing = true
        );
        table_row(table, "Q1", "Q2", "Q3", "Q4");
        table_row(table, "150", "220", "190", "310");
        table_print(table);
        table_free(table);
    }

    printf("\n=== %s ===\n", "Manual Initialization (table_init)");
    {
        TableConfig config = {
            .cell_padding = 1,
            .num_cols = 3,
            .output_stream = stdout,
            .output_format = TABLE_FMT_BORDERS,
            .border_style = TABLE_BORDER_DOUBLE,
        };
        Table *table = table_init(config);
        table_row(table, "Language", "Creator", "Year");
        table_row(table, "C", "Dennis Ritchie", "1972");
        table_row(table, "Python", "Guido van Rossum", "1991");
        table_row(table, "Rust", "Graydon Hoare", "2010");
        table_print(table);
        table_free(table);
    }

    printf("\n=== %s ===\n", "Right Alignment & Padding (Financial)");
    {
        Table *table = table_create(
            .num_cols = 2,
            .alignment = TABLE_ALIGN_RIGHT,
            .cell_padding = 2
        );
        table_row(table, "Description", "Balance");
        table_row(table, "Checking Account", "$1,240.50");
        table_row(table, "Savings Account", "$15,000.00");
        table_row(table, "Credit Card", "-$450.25");
        table_print(table);
        table_free(table);
    }

    printf("\n=== %s ===\n", "Centre Alignment & ASCII Style");
    {
        Table *table = table_create(
            .num_cols = 3,
            .border_style = TABLE_BORDER_ASCII,
            .alignment = TABLE_ALIGN_CENTRE
        );
        table_row(table, "Home", "vs", "Away");
        table_row(table, "Lions", "24 - 17", "Bears");
        table_row(table, "Eagles", "10 - 31", "Giants");
        table_print(table);
        table_free(table);
    }

    printf("\n=== %s ===\n", "Space-separated (Flush Left)");
    {
        Table *table = table_create(
            .num_cols = 3,
            .output_format = TABLE_FMT_SPACES,
            .cell_padding = 2
        );
        table_row(table, "UID", "GID", "HOME");
        table_row(table, "1000", "1000", "/home/devuser");
        table_row(table, "0", "0", "/root");
        table_print(table);
        table_free(table);
    }

    printf("\n=== %s ===\n", "CSV Export Format");
    {
        Table *table = table_create(.num_cols = 3, .output_format = TABLE_FMT_CSV);
        table_row(table, "id", "hex_code", "label");
        table_row(table, "color_01", "#FF5733", "Persimmon");
        table_row(table, "color_02", "#33FF57", "Emerald");
        table_row(table, "color_03", "#3357FF", "Royal Blue");
        table_print(table);
        table_free(table);
    }

    printf("\n=== %s ===\n", "Markdown Pipe Format");
    {
        Table *table = table_create(.num_cols = 3, .output_format = TABLE_FMT_PIPE);
        table_row(table, "Feature", "Version", "Released");
        table_row(table, "Async Engine", "v2.1.0", "2023-11-01");
        table_row(table, "Web Portal", "v1.4.2", "2024-01-15");
        table_print(table);
        table_free(table);
    }

    printf("\n=== %s ===\n", "Using table_row_array()");
    {
        Table *table = table_create(.num_cols = 2, .cell_padding = 1);
        const char *headers[] = {"Key", "Secret"};
        const char *data_pair[] = {"API_KEY_PUBLIC", "8fb23..9a1"};

        table_row_array(table, headers);
        table_row_array(table, data_pair);

        table_print(table);
        table_free(table);
    }

    return 0;
}
