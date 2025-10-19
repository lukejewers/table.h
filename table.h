#ifndef TABLE_H
#define TABLE_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_INIT_SIZE 64

typedef enum {
    BORDER_TOP_LEFT,
    BORDER_TOP_MIDDLE,
    BORDER_TOP_RIGHT,
    BORDER_MIDDLE_LEFT,
    BORDER_MIDDLE_MIDDLE,
    BORDER_MIDDLE_RIGHT,
    BORDER_BOTTOM_LEFT,
    BORDER_BOTTOM_MIDDLE,
    BORDER_BOTTOM_RIGHT,
    BORDER_HORIZONTAL,
    BORDER_VERTICAL,
    BORDER_COUNT
} BorderChar;

static char *BORDER_SETS[][BORDER_COUNT] = {
    // BORDER_SINGLE
    {"┌", "┬", "┐", "├", "┼", "┤", "└", "┴", "┘", "─", "│"},
    // BORDER_DOUBLE
    {"╔", "╦", "╗", "╠", "╬", "╣", "╚", "╩", "╝", "═", "║"},
    // BORDER_ASCII
    {"+", "+", "+", "+", "+", "+", "+", "+", "+", "-", "|"},
    // BORDER_ROUND
    {"╭", "┬", "╮", "├", "┼", "┤", "╰", "┴", "╯", "─", "│"}
};

typedef enum {
    FORMAT_CSV,      // Comma-separated values
    FORMAT_SPACES,   // Space-separated
    FORMAT_BORDERS,  // Bordered table
} OutputFormat;

typedef enum {
    BORDER_SINGLE,   // Single  border: ┌────┐
    BORDER_DOUBLE,   // Double  border: ╔════╗
    BORDER_ASCII,    // Ascii   border: +----+
    BORDER_ROUND,    // Rounded border: ╭────╮
    BORDER_NONE,     // No      border
} BorderStyle;

typedef struct {
    FILE *output_stream;
    OutputFormat output_format;
    BorderStyle border_style;
    bool even_col_spacing;
    unsigned int cell_padding;
    unsigned int num_cols;
} TableConfig;

typedef struct {
    TableConfig config;
    unsigned int rows_buffer_size;
    char ***rows_buffer;
    unsigned int num_rows;
} Table;

static inline void free_table(Table *table)
{
    if (!table) return;
    if (table->rows_buffer) {
        for (unsigned int i = 0; i < table->num_rows; i++) {
            free(table->rows_buffer[i]);
        }
        free(table->rows_buffer);
    }
    free(table);
}

static inline Table *table_init(TableConfig config)
{
    Table *table = malloc(sizeof(Table));
    if (table == NULL) {
        fprintf(stderr, "Failed to malloc Table");
        return NULL;
    }

    table->rows_buffer_size = TABLE_INIT_SIZE;
    table->rows_buffer = malloc(sizeof(char **) * TABLE_INIT_SIZE);
    if (table->rows_buffer == NULL) {
        fprintf(stderr, "Failed to malloc Table row_buffer");
        free(table);
        return NULL;
    }

    table->num_rows = 0;
    table->config = config;

    return table;
}

static inline void table_row(Table *table, ...)
{
    if (!table) return;
    if (table->num_rows >= table->rows_buffer_size) {
        table->rows_buffer_size *= 2;
        char ***new_buffer = realloc(table->rows_buffer, sizeof(char **) * table->rows_buffer_size);
        if (!new_buffer) {
            fprintf(stderr, "Failed to resize table buffer\n");
            return;
        }
        table->rows_buffer = new_buffer;
    }

    va_list args;
    va_start(args, table);

    char **new_row = malloc(sizeof(char *) * table->config.num_cols);
    if (new_row == NULL) {
        fprintf(stderr, "Failed to allocate row\n");
        va_end(args);
        return;
    }

    for (unsigned int i = 0; i < table->config.num_cols; i++) {
        new_row[i] = va_arg(args, char *);
    }

    table->rows_buffer[table->num_rows] = new_row;
    table->num_rows++;

    va_end(args);
}

static inline void table_row_array(Table *table, const char **values)
{
    if (!table) return;
    if (table->num_rows >= table->rows_buffer_size) {
        table->rows_buffer_size *= 2;
        char ***new_buffer = realloc(table->rows_buffer, sizeof(char **) * table->rows_buffer_size);
        if (!new_buffer) {
            fprintf(stderr, "Failed to resize table buffer\n");
            return;
        }
        table->rows_buffer = new_buffer;
    }

    char **new_row = malloc(sizeof(char *) * table->config.num_cols);
    if (!new_row) {
        fprintf(stderr, "Failed to allocate row\n");
        return;
    }

    for (unsigned int i = 0; i < table->config.num_cols; i++) {
        new_row[i] = (char *)values[i];
    }

    table->rows_buffer[table->num_rows] = new_row;
    table->num_rows++;
}


static inline void calculate_col_widths(const Table *table, unsigned int *col_widths)
{
    unsigned int max_col_width = 0;
    for (unsigned int col = 0; col < table->config.num_cols; ++col) {
        unsigned int max_str_len = 0;
        for (unsigned int row = 0; row < table->num_rows; ++row) {
            unsigned int value_len = strlen(table->rows_buffer[row][col]);
            if (value_len > max_str_len) max_str_len = value_len;
        }
        col_widths[col] = max_str_len;
        if (max_str_len > max_col_width) max_col_width = max_str_len;
    }
    if (table->config.even_col_spacing) {
        for (unsigned int i = 0; i < table->config.num_cols; ++i) col_widths[i] = max_col_width;
    }
}

static inline void table_print_csv(const Table *table)
{
    if (!table || !table->config.output_stream) return;

    for (unsigned int row = 0; row < table->num_rows; ++row) {
        for (unsigned int col = 0; col < table->config.num_cols; ++col) {
            fprintf(table->config.output_stream, "%s", table->rows_buffer[row][col]);
            if (col < table->config.num_cols - 1) fprintf(table->config.output_stream, ",");
        }
        fprintf(table->config.output_stream, "\n");
    }
}

static inline void table_print_border_line(const Table *table, unsigned int *col_widths, BorderChar left, BorderChar  centre, BorderChar right)
{
    fprintf(table->config.output_stream, "%s", BORDER_SETS[table->config.border_style][left]);
    for (unsigned int col = 0; col < table->config.num_cols; ++col) {
        for (unsigned int i = 0; i < col_widths[col] + (table->config.cell_padding * 2); ++i) {
            fprintf(table->config.output_stream, "%s", BORDER_SETS[table->config.border_style][BORDER_HORIZONTAL]);
        }
        if (col < table->config.num_cols - 1) fprintf(table->config.output_stream, "%s", BORDER_SETS[table->config.border_style][centre]);
        else fprintf(table->config.output_stream, "%s", BORDER_SETS[table->config.border_style][right]);
    }
    fprintf(table->config.output_stream, "\n");
}

static inline void table_print_bordered(const Table *table)
{
    if (!table || !table->config.output_stream) return;

    unsigned int col_widths[table->config.num_cols];
    calculate_col_widths(table, col_widths);
    // print top border
    table_print_border_line(table, col_widths, BORDER_TOP_LEFT, BORDER_TOP_MIDDLE, BORDER_TOP_RIGHT);
    // print rows with content
    for (unsigned int row = 0; row < table->num_rows; ++row) {
        fprintf(table->config.output_stream, "%s", BORDER_SETS[table->config.border_style][BORDER_VERTICAL]);
        for (unsigned int col = 0; col < table->config.num_cols; ++col) {
            // left padding
            for (unsigned int i = 0; i < table->config.cell_padding; ++i) {
                fprintf(table->config.output_stream, " ");
            }
            // cell content
            fprintf(table->config.output_stream, "%s", table->rows_buffer[row][col]);
            // right padding + alignment
            unsigned int padding = col_widths[col] - strlen(table->rows_buffer[row][col]);
            for (unsigned int i = 0; i < padding + table->config.cell_padding; ++i) {
                fprintf(table->config.output_stream, " ");
            }
            if (col < table->config.num_cols - 1) fprintf(table->config.output_stream, "%s", BORDER_SETS[table->config.border_style][BORDER_VERTICAL]);
            else fprintf(table->config.output_stream, "%s", BORDER_SETS[table->config.border_style][BORDER_VERTICAL]);
        }
        fprintf(table->config.output_stream, "\n");
        // print middle separator after each row (except last)
        if (row < table->num_rows - 1) {
            table_print_border_line(table, col_widths, BORDER_MIDDLE_LEFT, BORDER_MIDDLE_MIDDLE, BORDER_MIDDLE_RIGHT);
        }
    }
    // print bottom border
    table_print_border_line(table, col_widths, BORDER_BOTTOM_LEFT, BORDER_BOTTOM_MIDDLE, BORDER_BOTTOM_RIGHT);
}

static inline void table_print_spaces(const Table *table)
{
    if (!table || !table->config.output_stream) return;

    unsigned int col_widths[table->config.num_cols];
    calculate_col_widths(table, col_widths);

    for (unsigned int row = 0; row < table->num_rows; ++row) {
        for (unsigned int col = 0; col < table->config.num_cols; ++col) {
            fprintf(table->config.output_stream, "%s", table->rows_buffer[row][col]);
            if (col < table->config.num_cols - 1) {
                unsigned int curr_cell_len = strlen(table->rows_buffer[row][col]);
                unsigned int padding = col_widths[col] - curr_cell_len;
                for (unsigned int i = 0; i < padding + 1; ++i) {
                    fprintf(table->config.output_stream, " ");
                }
            }
        }
        fprintf(table->config.output_stream, "\n");
    }
}


static inline void table_print(const Table *table)
{
    if (!table) return;

    switch (table->config.output_format) {
    case FORMAT_CSV:
        table_print_csv(table);
        break;
    case FORMAT_SPACES:
        table_print_spaces(table);
        break;
    case FORMAT_BORDERS:
        table_print_bordered(table);
        break;
    }
}

#endif // TABLE_H
