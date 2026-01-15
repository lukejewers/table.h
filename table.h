#ifndef TABLE_H
#define TABLE_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
** Public types
*/

typedef enum {
    FORMAT_BORDERS,  /* Bordered table */
    FORMAT_CSV,      /* Comma-separated values */
    FORMAT_SPACES,   /* Space-separated */
} OutputFormat;

typedef enum {
    BORDER_SINGLE,   /* Single border:  ┌────┐ */
    BORDER_DOUBLE,   /* Double border:  ╔════╗ */
    BORDER_ASCII,    /* Ascii border:   +----+ */
    BORDER_ROUND,    /* Rounded border: ╭────╮ */
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

/*
** Internal types and data
*/

#define TABLE__INIT_SIZE 64

typedef enum {
    TABLE__BORDER_TOP_LEFT,
    TABLE__BORDER_TOP_MIDDLE,
    TABLE__BORDER_TOP_RIGHT,
    TABLE__BORDER_MIDDLE_LEFT,
    TABLE__BORDER_MIDDLE_MIDDLE,
    TABLE__BORDER_MIDDLE_RIGHT,
    TABLE__BORDER_BOTTOM_LEFT,
    TABLE__BORDER_BOTTOM_MIDDLE,
    TABLE__BORDER_BOTTOM_RIGHT,
    TABLE__BORDER_HORIZONTAL,
    TABLE__BORDER_VERTICAL,
    TABLE__BORDER_COUNT
} Table__BorderChar;

static const char * const TABLE__BORDER_SETS[][TABLE__BORDER_COUNT] = {
    {"┌", "┬", "┐", "├", "┼", "┤", "└", "┴", "┘", "─", "│"}, /* BORDER_SINGLE */
    {"╔", "╦", "╗", "╠", "╬", "╣", "╚", "╩", "╝", "═", "║"}, /* BORDER_DOUBLE */
    {"+", "+", "+", "+", "+", "+", "+", "+", "+", "-", "|"}, /* BORDER_ASCII */
    {"╭", "┬", "╮", "├", "┼", "┤", "╰", "┴", "╯", "─", "│"}  /* BORDER_ROUND */
};

/*
** Internal functions
*/

static inline bool table__realloc(Table *table)
{
    if (table->rows_buffer_count + table->config.num_cols > table->rows_buffer_capacity) {
        unsigned int new_cap = table->rows_buffer_capacity * 2;
        const char **new_buffer = realloc(table->rows_buffer, sizeof(char *) * new_cap);
        if (!new_buffer) return false;
        table->rows_buffer = new_buffer;
        table->rows_buffer_capacity = new_cap;
    }
    return true;
}

static inline size_t table__visible_length(const char *str) {
    size_t len = 0;
    int in_escape = false;
    for (const char *p = str; *p; ++p) {
        if (*p == '\033') in_escape = true;
        else if (in_escape && *p == 'm') in_escape = false;
        else if (!in_escape) ++len;
    }
    return len;
}

static inline void table__calc_col_widths(const Table *table, size_t *col_widths)
{
    unsigned int max_col_width = 0;
    for (unsigned int col = 0; col < table->config.num_cols; ++col) {
        unsigned int max_str_len = 0;
        for (unsigned int row = 0; row < table->num_rows; ++row) {
            size_t value_len = table__visible_length(table->rows_buffer[row * table->config.num_cols + col]);
            if (value_len > max_str_len) max_str_len = value_len;
        }
        col_widths[col] = max_str_len;
        if (max_str_len > max_col_width) max_col_width = max_str_len;
    }
    if (table->config.even_col_spacing) {
        for (unsigned int i = 0; i < table->config.num_cols; ++i) col_widths[i] = max_col_width;
    }
}

static inline bool table__csv_needs_escape(const char *value)
{
    for (const char *p = value; *p; ++p) {
        if (*p == ',' || *p == '"' || *p == '\n' || *p == '\r') return true;
    }
    return false;
}

static inline void table__print_csv_value(FILE *stream, const char *value)
{
    if (!value) value = "";
    if (!table__csv_needs_escape(value)) {
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

static inline void table__print_csv(const Table *table)
{
    if (!table || !table->config.output_stream) return;

    for (unsigned int row = 0; row < table->num_rows; ++row) {
        for (unsigned int col = 0; col < table->config.num_cols; ++col) {
            table__print_csv_value(table->config.output_stream, table->rows_buffer[row * table->config.num_cols + col]);
            if (col < table->config.num_cols - 1) fputs(",", table->config.output_stream);
        }
        fputs("\n", table->config.output_stream);
    }
}

static inline unsigned int table__get_border_style(const Table *table)
{
    return table->config.border_style < 4 ? table->config.border_style : 0;
}

static inline void table__print_border_line(const Table *table, size_t *col_widths, Table__BorderChar left, Table__BorderChar centre, Table__BorderChar right)
{
    size_t style = table__get_border_style(table);
    fputs(TABLE__BORDER_SETS[style][left], table->config.output_stream);
    for (unsigned int col = 0; col < table->config.num_cols; ++col) {
        for (unsigned int i = 0; i < col_widths[col] + (table->config.cell_padding * 2); ++i) {
            fputs(TABLE__BORDER_SETS[style][TABLE__BORDER_HORIZONTAL], table->config.output_stream);
        }
        if (col < table->config.num_cols - 1) fputs(TABLE__BORDER_SETS[style][centre], table->config.output_stream);
        else fputs(TABLE__BORDER_SETS[style][right], table->config.output_stream);
    }
    fputc('\n', table->config.output_stream);
}

static inline void table__print_bordered(const Table *table)
{
    if (!table || !table->config.output_stream) return;

    size_t *col_widths = malloc(sizeof(size_t) * table->config.num_cols);
    if (!col_widths) return;

    table__calc_col_widths(table, col_widths);
    table__print_border_line(table, col_widths, TABLE__BORDER_TOP_LEFT, TABLE__BORDER_TOP_MIDDLE, TABLE__BORDER_TOP_RIGHT);

    unsigned int style = table__get_border_style(table);

    for (unsigned int row = 0; row < table->num_rows; ++row) {
        fputs(TABLE__BORDER_SETS[style][TABLE__BORDER_VERTICAL], table->config.output_stream);
        for (unsigned int col = 0; col < table->config.num_cols; ++col) {
            for (unsigned int i = 0; i < table->config.cell_padding; ++i) {
                fputc(' ', table->config.output_stream);
            }
            fputs(table->rows_buffer[row * table->config.num_cols + col], table->config.output_stream);
            size_t padding = col_widths[col] - table__visible_length(table->rows_buffer[row * table->config.num_cols + col]);
            for (unsigned int i = 0; i < padding + table->config.cell_padding; ++i) {
                fputc(' ', table->config.output_stream);
            }
            fputs(TABLE__BORDER_SETS[style][TABLE__BORDER_VERTICAL], table->config.output_stream);
        }
        fputc('\n', table->config.output_stream);
        if (row < table->num_rows - 1) {
            table__print_border_line(table, col_widths, TABLE__BORDER_MIDDLE_LEFT, TABLE__BORDER_MIDDLE_MIDDLE, TABLE__BORDER_MIDDLE_RIGHT);
        }
    }

    table__print_border_line(table, col_widths, TABLE__BORDER_BOTTOM_LEFT, TABLE__BORDER_BOTTOM_MIDDLE, TABLE__BORDER_BOTTOM_RIGHT);
    free(col_widths);
}

static inline void table__print_spaces(const Table *table)
{
    if (!table || !table->config.output_stream) return;

    size_t *col_widths = malloc(sizeof(size_t) * table->config.num_cols);
    if (!col_widths) return;

    table__calc_col_widths(table, col_widths);

    for (unsigned int row = 0; row < table->num_rows; ++row) {
        for (unsigned int col = 0; col < table->config.num_cols; ++col) {
            fputs(table->rows_buffer[row * table->config.num_cols + col], table->config.output_stream);
            if (col < table->config.num_cols - 1) {
                size_t curr_cell_len = table__visible_length(table->rows_buffer[row * table->config.num_cols + col]);
                size_t padding = col_widths[col] - curr_cell_len;
                for (size_t i = 0; i < padding + 1; ++i) {
                    fputc(' ', table->config.output_stream);
                }
            }
        }
        fputc('\n', table->config.output_stream);
    }

    free(col_widths);
}


/*
** Public API
*/

static inline Table *table_init(TableConfig config)
{
    if (config.num_cols <= 0) return NULL;

    Table *table = malloc(sizeof(Table));
    if (!table) return NULL;

    table->rows_buffer_capacity = TABLE__INIT_SIZE;
    table->rows_buffer_count = 0;
    table->num_rows = 0;
    table->config = config;
    if (!table->config.output_stream) table->config.output_stream = stdout;
    if (table->config.cell_padding == 0) table->config.cell_padding = 1;

    table->rows_buffer = malloc(sizeof(char *) * table->rows_buffer_capacity);
    if (!table->rows_buffer) {
        free(table);
        return NULL;
    }

    return table;
}

/* Creates a new table with mandatory positional argument for cols and optional named arguments */
#define table_create(...) table_init((TableConfig){ __VA_ARGS__ })

/* Removes all rows from the table, keeping the configuration */
static inline void table_clear(Table *table)
{
    if (!table) return;
    table->rows_buffer_count = 0;
    table->num_rows = 0;
}

/* Frees all memory associated with the table. */
static inline void table_free(Table *table)
{
    if (!table) return;
    if (table->rows_buffer) free(table->rows_buffer);
    free(table);
}

/*
** Adds a row using variadic arguments. Pass exactly num_cols strings.
**
** NOTE: Strings are NOT copied. The table stores pointers to your strings.
** Ensure the strings remain valid until after table_print() or table_free() is called.
*/
static inline bool table_row(Table *table, ...)
{
    if (!table) return false;

    if (!table__realloc(table)) return false;

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

/*
** Adds a row from a string array of length num_cols.
**
** NOTE: Strings are NOT copied. The table stores pointers to your strings.
** Ensure the strings remain valid until after table_print() or table_free() is called.
**/
static inline bool table_row_array(Table *table, const char **values)
{
    if (!table || !values) return false;

    if (!table__realloc(table)) return false;

    for (unsigned int i = 0; i < table->config.num_cols; ++i) {
        table->rows_buffer[table->rows_buffer_count++] = values[i] ? values[i] : "";
    }
    table->num_rows++;
    return true;
}

/* Prints the table to the configured output stream. */
static inline void table_print(const Table *table)
{
    if (!table) return;

    switch (table->config.output_format) {
    case FORMAT_CSV:
        table__print_csv(table);
        break;
    case FORMAT_SPACES:
        table__print_spaces(table);
        break;
    case FORMAT_BORDERS:
        table__print_bordered(table);
        break;
    }
}

#endif /* TABLE_H */
