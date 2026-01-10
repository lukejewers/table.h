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

static const char * const BORDER_SETS[][BORDER_COUNT] = {
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
    FORMAT_BORDERS,  // Bordered table
    FORMAT_CSV,      // Comma-separated values
    FORMAT_SPACES,   // Space-separated
} OutputFormat;

typedef enum {
    BORDER_SINGLE,   // Single border:  ┌────┐
    BORDER_DOUBLE,   // Double border:  ╔════╗
    BORDER_ASCII,    // Ascii border:   +----+
    BORDER_ROUND,    // Rounded border: ╭────╮
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
    const char **rows_buffer;
    unsigned int rows_buffer_capacity;
    unsigned int rows_buffer_count;
    unsigned int num_rows;
} Table;

static inline void free_table(Table *table)
{
    if (!table) return;
    if (table->rows_buffer) free(table->rows_buffer);
    free(table);
}

static inline Table *table_init(TableConfig config)
{
    Table *table = malloc(sizeof(Table));
    if (table == NULL) return NULL;

    table->rows_buffer_capacity = TABLE_INIT_SIZE;
    table->rows_buffer_count = 0;
    table->num_rows = 0;
    table->config = config;

    table->rows_buffer = malloc(sizeof(char *) * table->rows_buffer_capacity);
    if (table->rows_buffer == NULL) {
        free(table);
        return NULL;
    }

    return table;
}

static inline bool table_realloc(Table *table)
{
    if (table->rows_buffer_count + table->config.num_cols > table->rows_buffer_capacity) {
        table->rows_buffer_capacity *= 2;
        const char **new_buffer = realloc(table->rows_buffer, sizeof(char *) * table->rows_buffer_capacity);
        if (!new_buffer) {
            fputs("table.h: error: failed to resize\n", stderr);
            return false;
        }
        table->rows_buffer = new_buffer;
    }
    return true;
}

static inline bool table_row(Table *table, ...)
{
    if (!table) return false;

    if (!table_realloc(table)) return false;

    va_list args;
    va_start(args, table);
    for (unsigned int i = 0; i < table->config.num_cols; ++i) {
        const char *value = va_arg(args, char *);
        table->rows_buffer[table->rows_buffer_count++] = value ? value : "";
    }
    table->num_rows++;
    va_end(args);
    return true;
}

static inline bool table_row_array(Table *table, const char **values)
{
    if (!table || !values) return false;

    if (!table_realloc(table)) return false;

    for (unsigned int i = 0; i < table->config.num_cols; ++i) {
        table->rows_buffer[table->rows_buffer_count++] = values[i] ? values[i] : "";
    }
    table->num_rows++;
    return true;
}

static inline void calculate_col_widths(const Table *table, unsigned int *col_widths)
{
    unsigned int max_col_width = 0;
    for (unsigned int col = 0; col < table->config.num_cols; ++col) {
        unsigned int max_str_len = 0;
        for (unsigned int row = 0; row < table->num_rows; ++row) {
            unsigned int value_len = strlen(table->rows_buffer[row * table->config.num_cols + col]);
            if (value_len > max_str_len) max_str_len = value_len;
        }
        col_widths[col] = max_str_len;
        if (max_str_len > max_col_width) max_col_width = max_str_len;
    }
    if (table->config.even_col_spacing) {
        for (unsigned int i = 0; i < table->config.num_cols; ++i) col_widths[i] = max_col_width;
    }
}

static inline bool table_csv_needs_escape(const char *value)
{
    for (const char *p = value; *p; ++p) {
        if (*p == ',' || *p == '"' || *p == '\n' || *p == '\r') return true;
    }
    return false;
}

static inline void table_print_csv_value(FILE *stream, const char *value)
{
    if (!table_csv_needs_escape(value)) {
        fputs(value, stream);
        return;
    }
    fputc('"', stream);
    for (const char *p = value; *p; ++p) {
        if (*p == '"') fputc('"', stream);
        fputc(*p, stream);
    }
    fputc('"', stream);
}

static inline void table_print_csv(const Table *table)
{
    if (!table || !table->config.output_stream) return;

    for (unsigned int row = 0; row < table->num_rows; ++row) {
        for (unsigned int col = 0; col < table->config.num_cols; ++col) {
            table_print_csv_value(table->config.output_stream, table->rows_buffer[row * table->config.num_cols + col]);
            if (col < table->config.num_cols - 1) fputs(",", table->config.output_stream);
        }
        fputs("\n", table->config.output_stream);
    }
}

static inline unsigned int table_get_border_style(const Table *table)
{
    return table->config.border_style < 4 ? table->config.border_style : 0;
}

static inline void table_print_border_line(const Table *table, unsigned int *col_widths, BorderChar left, BorderChar centre, BorderChar right)
{
    unsigned int style = table_get_border_style(table);
    fputs(BORDER_SETS[style][left], table->config.output_stream);
    for (unsigned int col = 0; col < table->config.num_cols; ++col) {
        for (unsigned int i = 0; i < col_widths[col] + (table->config.cell_padding * 2); ++i) {
            fputs(BORDER_SETS[style][BORDER_HORIZONTAL], table->config.output_stream);
        }
        if (col < table->config.num_cols - 1) fputs(BORDER_SETS[style][centre], table->config.output_stream);
        else fputs(BORDER_SETS[style][right], table->config.output_stream);
    }
    fputc('\n', table->config.output_stream);
}

static inline void table_print_bordered(const Table *table)
{
    if (!table || !table->config.output_stream) return;

    unsigned int col_widths[table->config.num_cols];
    calculate_col_widths(table, col_widths);
    // print top border
    table_print_border_line(table, col_widths, BORDER_TOP_LEFT, BORDER_TOP_MIDDLE, BORDER_TOP_RIGHT);

    unsigned int style = table_get_border_style(table);

    // print rows with content
    for (unsigned int row = 0; row < table->num_rows; ++row) {
        fputs(BORDER_SETS[style][BORDER_VERTICAL], table->config.output_stream);
        for (unsigned int col = 0; col < table->config.num_cols; ++col) {
            // left padding
            for (unsigned int i = 0; i < table->config.cell_padding; ++i) {
                fputc(' ', table->config.output_stream);
            }
            // cell content
            fputs(table->rows_buffer[row * table->config.num_cols + col], table->config.output_stream);
            // right padding + alignment
            unsigned int padding = col_widths[col] - strlen(table->rows_buffer[row * table->config.num_cols + col]);
            for (unsigned int i = 0; i < padding + table->config.cell_padding; ++i) {
                fputc(' ', table->config.output_stream);
            }
            fputs(BORDER_SETS[style][BORDER_VERTICAL], table->config.output_stream);
        }
        fputc('\n', table->config.output_stream);
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
            fputs(table->rows_buffer[row * table->config.num_cols + col], table->config.output_stream);
            if (col < table->config.num_cols - 1) {
                unsigned int curr_cell_len = strlen(table->rows_buffer[row * table->config.num_cols + col]);
                unsigned int padding = col_widths[col] - curr_cell_len;
                for (unsigned int i = 0; i < padding + 1; ++i) {
                    fputc(' ', table->config.output_stream);
                }
            }
        }
        fputc('\n', table->config.output_stream);
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
