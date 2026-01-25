#include "table_fmt.h"

int main(void)
{

    printf("\n=== %s ===\n", "Single Border (Default)");
    {
        TableFmt *table = table_fmt_create(.num_cols = 3);
        table_fmt_row(table, "Planet", "Diameter", "Type");
        table_fmt_row(table, "Jupiter", "139,820 km", "Gas Giant");
        table_fmt_row(table, "Mars", "6,779 km", "Terrestrial");
        table_fmt_row(table, "Neptune", "49,244 km", "Ice Giant");
        table_fmt_print(table);
        table_fmt_free(table);
    }

    printf("\n=== %s ===\n", "Grid Lines");
    {
        TableFmt *table = table_fmt_create(.num_cols = 3, .grid_lines = true);

        table_fmt_row(table, "Service", "Port", "Priority");
        table_fmt_row(table, "Database", "5432", "High");
        table_fmt_row(table, "Auth API", "8080", "Critical");
        table_fmt_row(table, "Cache", "6379", "Medium");

        table_fmt_print(table);
        table_fmt_free(table);
    }

    printf("\n=== %s ===\n", "Double Border & Rich Styling");
    {
        TableFmt *table = table_fmt_create(.num_cols = 3, .border_type = TABLE_FMT_BORDER_DOUBLE);
        table_fmt_row(table, TABLE_FMT_BOLD TABLE_FMT_UNDERLINE "Server" TABLE_FMT_RESET,
                         TABLE_FMT_BOLD TABLE_FMT_UNDERLINE "Uptime" TABLE_FMT_RESET,
                         TABLE_FMT_BOLD TABLE_FMT_UNDERLINE "Status" TABLE_FMT_RESET);
        table_fmt_row(table, "US-East-1", "142 days", TABLE_FMT_CELL_STYLE(TABLE_FMT_GREEN, "OPERATIONAL"));
        table_fmt_row(table, "EU-Central-1", "12 hours", TABLE_FMT_CELL_STYLE(TABLE_FMT_YELLOW, "DEGRADED"));
        table_fmt_row(table, "AP-South-1", "0 days", TABLE_FMT_CELL_STYLE(TABLE_FMT_RED, "OFFLINE"));

        table_fmt_print(table);
        table_fmt_free(table);
    }

    printf("\n=== %s ===\n", "Header Styling");
    {
        TableFmt *table = table_fmt_create(
            .num_cols = 3,
            .header_style = TABLE_FMT_BOLD TABLE_FMT_GREEN);

        table_fmt_row(table, "ID", "Library", "License");
        table_fmt_row(table, "001", "table_fmt.h", "MIT");
        table_fmt_row(table, "002", "Zlib", "Zlib");
        table_fmt_row(table, "003", "SQLite", "Public Domain");

        table_fmt_print(table);
        table_fmt_free(table);
    }

    printf("\n=== %s ===\n", "Rounded Border & Even Column Spacing");
    {
        TableFmt *table = table_fmt_create(
            .num_cols = 4,
            .border_type = TABLE_FMT_BORDER_ROUND,
            .even_col_spacing = true
        );
        table_fmt_row(table, "Q1", "Q2", "Q3", "Q4");
        table_fmt_row(table, "150", "220", "190", "310");
        table_fmt_print(table);
        table_fmt_free(table);
    }

    printf("\n=== %s ===\n", "Manual Initialization (table_fmt_init)");
    {
        TableFmtConfig config = {
            .cell_padding = 1,
            .num_cols = 3,
            .output_stream = stdout,
            .output_format = TABLE_FMT_OUTPUT_BORDERS,
            .border_type = TABLE_FMT_BORDER_DOUBLE,
        };
        TableFmt *table = table_fmt_init(config);
        table_fmt_row(table, "Language", "Creator", "Year");
        table_fmt_row(table, "C", "Dennis Ritchie", "1972");
        table_fmt_row(table, "Python", "Guido van Rossum", "1991");
        table_fmt_row(table, "Rust", "Graydon Hoare", "2010");
        table_fmt_print(table);
        table_fmt_free(table);
    }

    printf("\n=== %s ===\n", "Right Alignment & Padding (Financial)");
    {
        TableFmt *table = table_fmt_create(
            .num_cols = 2,
            .alignment = TABLE_FMT_ALIGN_RIGHT,
            .cell_padding = 2
        );
        table_fmt_row(table, "Description", "Balance");
        table_fmt_row(table, "Checking Account", "$1,240.50");
        table_fmt_row(table, "Savings Account", "$15,000.00");
        table_fmt_row(table, "Credit Card", "-$450.25");
        table_fmt_print(table);
        table_fmt_free(table);
    }

    printf("\n=== %s ===\n", "Centre Alignment & ASCII Style");
    {
        TableFmt *table = table_fmt_create(
            .num_cols = 3,
            .border_type = TABLE_FMT_BORDER_ASCII,
            .alignment = TABLE_FMT_ALIGN_CENTRE
        );
        table_fmt_row(table, "Home", "vs", "Away");
        table_fmt_row(table, "Lions", "24 - 17", "Bears");
        table_fmt_row(table, "Eagles", "10 - 31", "Giants");
        table_fmt_print(table);
        table_fmt_free(table);
    }

    printf("\n=== %s ===\n", "Space-separated (Flush Left)");
    {
        TableFmt *table = table_fmt_create(
            .num_cols = 3,
            .output_format = TABLE_FMT_OUTPUT_SPACE,
            .cell_padding = 2
        );
        table_fmt_row(table, "UID", "GID", "HOME");
        table_fmt_row(table, "1000", "1000", "/home/devuser");
        table_fmt_row(table, "0", "0", "/root");
        table_fmt_print(table);
        table_fmt_free(table);
    }

    printf("\n=== %s ===\n", "CSV Export Format");
    {
        TableFmt *table = table_fmt_create(.num_cols = 3, .output_format = TABLE_FMT_OUTPUT_CSV);
        table_fmt_row(table, "id", "hex_code", "label");
        table_fmt_row(table, "color_01", "#FF5733", "Persimmon");
        table_fmt_row(table, "color_02", "#33FF57", "Emerald");
        table_fmt_row(table, "color_03", "#3357FF", "Royal Blue");
        table_fmt_print(table);
        table_fmt_free(table);
    }

    printf("\n=== %s ===\n", "Markdown Pipe Format");
    {
        TableFmt *table = table_fmt_create(.num_cols = 3, .output_format = TABLE_FMT_OUTPUT_PIPE);
        table_fmt_row(table, "Feature", "Version", "Released");
        table_fmt_row(table, "Async Engine", "v2.1.0", "2023-11-01");
        table_fmt_row(table, "Web Portal", "v1.4.2", "2024-01-15");
        table_fmt_print(table);
        table_fmt_free(table);
    }

    printf("\n=== %s ===\n", "Using table_fmt_row_array()");
    {
        TableFmt *table = table_fmt_create(.num_cols = 2, .cell_padding = 1);
        const char *headers[] = {"Key", "Secret"};
        const char *data_pair[] = {"API_KEY_PUBLIC", "8fb23..9a1"};

        table_fmt_row_array(table, headers);
        table_fmt_row_array(table, data_pair);

        table_fmt_print(table);
        table_fmt_free(table);
    }

    return 0;
}
