#ifndef TABLE_H
#define TABLE_H

#include <stdarg.h>
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
} BorderStyle;

typedef struct {
    FILE *output_stream;
    OutputFormat output_format;
    BorderStyle border_style;
    unsigned int rows_buffer_size;
    char ***rows_buffer;
    unsigned int num_rows;
    unsigned int num_cols;
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

static inline Table *table_init(FILE *output_stream, OutputFormat output_format, BorderStyle border_style, unsigned int num_cols)
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
    table->num_cols = num_cols;
    table->output_stream = output_stream;
    table->output_format = output_format;
    table->border_style = border_style;

    return table;
}

static inline void table_row(Table *table, ...)
{
    if (!table || table->num_rows >= table->rows_buffer_size) {
        fprintf(stderr, "Buffer full!\n");
        return;
    }

    va_list args;
    va_start(args, table);

    char **new_row = malloc(sizeof(char *) * table->num_cols);
    if (new_row == NULL) {
        fprintf(stderr, "Failed to allocate row\n");
        va_end(args);
        return;
    }

    for (unsigned int i = 0; i < table->num_cols; i++) {
        new_row[i] = va_arg(args, char *);
    }

    table->rows_buffer[table->num_rows] = new_row;
    table->num_rows++;

    va_end(args);
}

static inline void calculate_col_widths(const Table *table, unsigned int *col_widths)
{
    for (unsigned int col = 0; col < table->num_cols; ++col) {
        unsigned int max_str_len = 0;
        for (unsigned int row = 0; row < table->num_rows; ++row) {
            unsigned int value_len = strlen(table->rows_buffer[row][col]);
            if (value_len > max_str_len) max_str_len = value_len;
        }
        col_widths[col] = max_str_len;
    }
}

static inline void table_print_csv(const Table *table)
{
    if (!table || !table->output_stream) return;

    for (unsigned int row = 0; row < table->num_rows; ++row) {
        for (unsigned int col = 0; col < table->num_cols; ++col) {
            fprintf(table->output_stream, "%s", table->rows_buffer[row][col]);
            if (col < table->num_cols - 1) fprintf(table->output_stream, ",");
        }
        fprintf(table->output_stream, "\n");
    }
}

static inline void table_print_bordered(const Table *table)
{
    if (!table || !table->output_stream) return;

    unsigned int col_widths[table->num_cols];
    calculate_col_widths(table, col_widths);
    // print top border
    fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_TOP_LEFT]);
    for (unsigned int col = 0; col < table->num_cols; ++col) {
        for (unsigned int i = 0; i < col_widths[col]; ++i) {
            fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_HORIZONTAL]);
        }
        if (col < table->num_cols - 1) fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_TOP_MIDDLE]);
        else fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_TOP_RIGHT]);
    }
    fprintf(table->output_stream, "\n");
    // print rows with content
    for (unsigned int row = 0; row < table->num_rows; ++row) {
        fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_VERTICAL]);
        for (unsigned int col = 0; col < table->num_cols; ++col) {
            fprintf(table->output_stream, "%s", table->rows_buffer[row][col]);
            unsigned int current_len = strlen(table->rows_buffer[row][col]);
            unsigned int padding = col_widths[col] - current_len;
            for (unsigned int i = 0; i < padding; ++i) {
                fprintf(table->output_stream, " ");
            }
            if (col < table->num_cols - 1) fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_VERTICAL]);
            else fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_VERTICAL]);
        }
        fprintf(table->output_stream, "\n");
        // print middle separator after each row (except last)
        if (row < table->num_rows - 1) {
            fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_MIDDLE_LEFT]);
            for (unsigned int col = 0; col < table->num_cols; ++col) {
                for (unsigned int i = 0; i < col_widths[col]; ++i) {
                    fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_HORIZONTAL]);
                }
                if (col < table->num_cols - 1) fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_MIDDLE_MIDDLE]);
                else fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_MIDDLE_RIGHT]);
            }
            fprintf(table->output_stream, "\n");
        }
    }
    // print bottom border
    fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_BOTTOM_LEFT]);
    for (unsigned int col = 0; col < table->num_cols; ++col) {
        for (unsigned int i = 0; i < col_widths[col]; ++i) {
            fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_HORIZONTAL]);
        }
        if (col < table->num_cols - 1) fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_BOTTOM_MIDDLE]);
        else fprintf(table->output_stream, "%s", BORDER_SETS[table->border_style][BORDER_BOTTOM_RIGHT]);
    }
    fprintf(table->output_stream, "\n");
}

static inline void table_print_spaces(const Table *table)
{
    if (!table || !table->output_stream) return;

    unsigned int col_widths[table->num_cols];
    calculate_col_widths(table, col_widths);

    for (unsigned int row = 0; row < table->num_rows; ++row) {
        for (unsigned int col = 0; col < table->num_cols; ++col) {
            fprintf(table->output_stream, "%s", table->rows_buffer[row][col]);
            if (col < table->num_cols - 1) {
                unsigned int curr_cell_len = strlen(table->rows_buffer[row][col]);
                unsigned int padding = col_widths[col] - curr_cell_len;
                for (unsigned int i = 0; i < padding + 1; ++i) {
                    fprintf(table->output_stream, " ");
                }
            }
        }
        fprintf(table->output_stream, "\n");
    }
}


static inline void table_print(const Table *table)
{
    if (!table) return;

    switch (table->output_format) {
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
