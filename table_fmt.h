#ifndef TABLE_FMT_H
#define TABLE_FMT_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
** Public types
*/

typedef enum {
    TABLE_FMT_OUTPUT_BORDERS,  /* Bordered table */
    TABLE_FMT_OUTPUT_CSV,      /* Comma-separated values */
    TABLE_FMT_OUTPUT_SPACE,    /* Space-separated */
    TABLE_FMT_OUTPUT_PIPE,     /* Pipe-delimited */
} TableFmtOutput;

typedef enum {
    TABLE_FMT_BORDER_SINGLE,   /* Single border:  ┌─────┐ */
    TABLE_FMT_BORDER_DOUBLE,   /* Double border:  ╔═════╗ */
    TABLE_FMT_BORDER_ROUND,    /* Rounded border: ╭─────╮ */
    TABLE_FMT_BORDER_ASCII,    /* Ascii border:   +-----+ */
    TABLE_FMT_BORDER_PIPE,     /* Pipe border:    | --- | */
} TableFmtBorderType;

typedef enum {
    TABLE_FMT_ALIGN_LEFT,
    TABLE_FMT_ALIGN_CENTRE,
    TABLE_FMT_ALIGN_RIGHT,
} TableFmtAlignment;

typedef struct {
    FILE *output_stream;
    const char *header_style;
    size_t cell_padding;
    size_t num_cols;
    TableFmtOutput output_format;
    TableFmtBorderType border_type;
    TableFmtAlignment alignment;
    bool even_col_spacing;
    bool grid_lines;
} TableFmtConfig;

typedef struct {
    TableFmtConfig config;
    const char **row_data;
    size_t row_data_capacity;
    size_t row_data_count;
    size_t num_rows;
} TableFmt;

/*
** Styling Macros (ANSI Escape Codes)
**
** 1. Macro: Use TABLE_FMT_CELL_STYLE(style, "text") for automatic resetting.
**           Note: 'style' can be one or more tags (e.g. TABLE_FMT_BOLD TABLE_FMT_RED).
** 2. Tags:  Stack tags like TABLE_FMT_BOLD TABLE_FMT_RED "text" TABLE_FMT_RESET.
**           IMPORTANT: When using tags manually, you MUST append TABLE_FMT_RESET
**           to prevent styles from "bleeding" into table borders or the terminal.
*/

#define TABLE_FMT_RESET     "\033[0m"

// Convenience macro
#define TABLE_FMT_CELL_STYLE(style, text) style text TABLE_FMT_RESET

// Text styles
#define TABLE_FMT_BOLD      "\033[1m"
#define TABLE_FMT_ITALIC    "\033[3m"
#define TABLE_FMT_UNDERLINE "\033[4m"

// Colors
#define TABLE_FMT_BLACK     "\033[30m"
#define TABLE_FMT_RED       "\033[31m"
#define TABLE_FMT_GREEN     "\033[32m"
#define TABLE_FMT_YELLOW    "\033[33m"
#define TABLE_FMT_BLUE      "\033[34m"
#define TABLE_FMT_MAGENTA   "\033[35m"
#define TABLE_FMT_CYAN      "\033[36m"
#define TABLE_FMT_WHITE     "\033[37m"

/*
** Internal types and data
*/

#define TABLE_FMT__INIT_SIZE 64

typedef enum {
    TABLE_FMT__BORDER_TOP_LEFT,
    TABLE_FMT__BORDER_TOP_MIDDLE,
    TABLE_FMT__BORDER_TOP_RIGHT,
    TABLE_FMT__BORDER_MIDDLE_LEFT,
    TABLE_FMT__BORDER_MIDDLE_MIDDLE,
    TABLE_FMT__BORDER_MIDDLE_RIGHT,
    TABLE_FMT__BORDER_BOTTOM_LEFT,
    TABLE_FMT__BORDER_BOTTOM_MIDDLE,
    TABLE_FMT__BORDER_BOTTOM_RIGHT,
    TABLE_FMT__BORDER_HORIZONTAL,
    TABLE_FMT__BORDER_VERTICAL,
    TABLE_FMT__BORDER_COUNT
} TableFmt__BorderChar;

static const char * const TABLE_FMT__BORDER_SETS[][TABLE_FMT__BORDER_COUNT] = {
    {"┌", "┬", "┐", "├", "┼", "┤", "└", "┴", "┘", "─", "│"}, /* TABLE_FMT_BORDER_SINGLE */
    {"╔", "╦", "╗", "╠", "╬", "╣", "╚", "╩", "╝", "═", "║"}, /* TABLE_FMT_BORDER_DOUBLE */
    {"╭", "┬", "╮", "├", "┼", "┤", "╰", "┴", "╯", "─", "│"}, /* TABLE_FMT_BORDER_ROUND */
    {"+", "+", "+", "+", "+", "+", "+", "+", "+", "-", "|"}, /* TABLE_FMT_BORDER_ASCII */
    {" ", " ", " ", "|", "|", "|", " ", " ", " ", "-", "|"}  /* TABLE_FMT_BORDER_PIPE */
};

typedef struct {
    size_t left;
    size_t right;
} TableFmt__CellPadding;

/*
** Internal functions
*/

static inline bool table_fmt__realloc(TableFmt *table)
{
    if (table->row_data_count + table->config.num_cols > table->row_data_capacity) {
        size_t new_cap = table->row_data_capacity * 2;
        const char **new_buffer = realloc(table->row_data, sizeof(char *) * new_cap);
        if (!new_buffer) return false;
        table->row_data = new_buffer;
        table->row_data_capacity = new_cap;
    }
    return true;
}

static inline size_t table_fmt__content_visible_length(const char *str) {
    size_t len = 0;
    int in_escape = false;
    for (const char *p = str; *p; ++p) {
        if (*p == '\033') in_escape = true;
        else if (in_escape && *p == 'm') in_escape = false;
        else if (!in_escape) ++len;
    }
    return len;
}

static inline void table_fmt__calc_col_widths(const TableFmt *table, size_t *col_widths)
{
    size_t max_col_width = 0;
    for (size_t col = 0; col < table->config.num_cols; ++col) {
        size_t max_str_len = 0;
        for (size_t row = 0; row < table->num_rows; ++row) {
            size_t value_len = table_fmt__content_visible_length(table->row_data[row * table->config.num_cols + col]);
            if (value_len > max_str_len) max_str_len = value_len;
        }
        col_widths[col] = max_str_len;
        if (max_str_len > max_col_width) max_col_width = max_str_len;
    }
    if (table->config.even_col_spacing) {
        for (size_t i = 0; i < table->config.num_cols; ++i) col_widths[i] = max_col_width;
    }
}

static inline bool table_fmt__csv_needs_escape(const char *value)
{
    for (const char *p = value; *p; ++p) {
        if (*p == ',' || *p == '"' || *p == '\n' || *p == '\r') return true;
    }
    return false;
}

static inline void table_fmt__print_csv_value(FILE *stream, const char *value)
{
    if (!value) value = "";
    if (!table_fmt__csv_needs_escape(value)) {
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

static inline void table_fmt__print_csv(const TableFmt *table)
{
    if (!table || !table->config.output_stream) return;

    for (size_t row = 0; row < table->num_rows; ++row) {
        for (size_t col = 0; col < table->config.num_cols; ++col) {
            table_fmt__print_csv_value(table->config.output_stream, table->row_data[row * table->config.num_cols + col]);
            if (col < table->config.num_cols - 1) fputs(",", table->config.output_stream);
        }
        fputs("\n", table->config.output_stream);
    }
}

static inline size_t table_fmt__get_border_type(const TableFmt *table)
{
    if (table->config.output_format == TABLE_FMT_OUTPUT_PIPE) return TABLE_FMT_BORDER_PIPE;
    return table->config.border_type < 5 ? table->config.border_type : 0;
}

static inline void table_fmt__print_border_line(const TableFmt *table, size_t *col_widths, size_t style, TableFmt__BorderChar left, TableFmt__BorderChar centre, TableFmt__BorderChar right)
{
    fputs(TABLE_FMT__BORDER_SETS[style][left], table->config.output_stream);
    for (size_t col = 0; col < table->config.num_cols; ++col) {
        for (size_t i = 0; i < col_widths[col] + (table->config.cell_padding * 2); ++i) {
            if (style == TABLE_FMT_BORDER_PIPE && (i == 0 || i == col_widths[col] + (table->config.cell_padding * 2) - 1)) {
                fputs(" ", table->config.output_stream);
                continue;
            };
            fputs(TABLE_FMT__BORDER_SETS[style][TABLE_FMT__BORDER_HORIZONTAL], table->config.output_stream);
        }
        if (col < table->config.num_cols - 1) fputs(TABLE_FMT__BORDER_SETS[style][centre], table->config.output_stream);
        else fputs(TABLE_FMT__BORDER_SETS[style][right], table->config.output_stream);
    }
    fputc('\n', table->config.output_stream);
}

static inline TableFmt__CellPadding table_fmt__get_cell_padding(const TableFmt *table, size_t total_cell_width, size_t content_len)
{
    TableFmt__CellPadding cell_padding = {0};
    size_t empty_space = total_cell_width - content_len;

    switch (table->config.alignment) {
    case TABLE_FMT_ALIGN_RIGHT:
        cell_padding.right = table->config.cell_padding;
        cell_padding.left = empty_space - cell_padding.right;
        break;
    case TABLE_FMT_ALIGN_CENTRE:
        cell_padding.left = empty_space / 2;
        cell_padding.right = empty_space - cell_padding.left;
        break;
    case TABLE_FMT_ALIGN_LEFT:
    default:
        cell_padding.left = table->config.cell_padding;
        cell_padding.right = empty_space - cell_padding.left;
        break;
    }
    return cell_padding;
}

static inline void table_fmt__print_bordered(const TableFmt *table)
{
    if (!table || !table->config.output_stream) return;

    size_t *col_widths = malloc(sizeof(size_t) * table->config.num_cols);
    if (!col_widths) return;
    table_fmt__calc_col_widths(table, col_widths);

    size_t style = table_fmt__get_border_type(table);

    if (table->config.output_format != TABLE_FMT_OUTPUT_PIPE) {
        table_fmt__print_border_line(table, col_widths, style, TABLE_FMT__BORDER_TOP_LEFT, TABLE_FMT__BORDER_TOP_MIDDLE, TABLE_FMT__BORDER_TOP_RIGHT);
    }

    for (size_t row = 0; row < table->num_rows; ++row) {
        fputs(TABLE_FMT__BORDER_SETS[style][TABLE_FMT__BORDER_VERTICAL], table->config.output_stream);
        for (size_t col = 0; col < table->config.num_cols; ++col) {
            size_t content_len = table_fmt__content_visible_length(table->row_data[row * table->config.num_cols + col]);
            size_t padding_multiplier = 2;
            size_t total_cell_width = col_widths[col] + (table->config.cell_padding * padding_multiplier);
            TableFmt__CellPadding cell_padding = table_fmt__get_cell_padding(table, total_cell_width, content_len);
            for (size_t i = 0; i < cell_padding.left; ++i) fputc(' ', table->config.output_stream);

            const char *content = table->row_data[row * table->config.num_cols + col];
            const char* ansi_start = (row == 0) ? (table->config.header_style ? table->config.header_style : "") : "";
            const char* ansi_end = (row == 0 && table->config.header_style) ? TABLE_FMT_RESET : "";

            fprintf(table->config.output_stream, "%s%s%s", ansi_start, content, ansi_end);

            for (size_t i = 0; i < cell_padding.right; ++i) fputc(' ', table->config.output_stream);
            fputs(TABLE_FMT__BORDER_SETS[style][TABLE_FMT__BORDER_VERTICAL], table->config.output_stream);
        }
        fputc('\n', table->config.output_stream);
        if (row < table->num_rows - 1) {
            if (table->config.output_format == TABLE_FMT_OUTPUT_PIPE && row != 0)  continue;
            if (row == 0 || table->config.grid_lines) {
                table_fmt__print_border_line(table, col_widths, style, TABLE_FMT__BORDER_MIDDLE_LEFT, TABLE_FMT__BORDER_MIDDLE_MIDDLE, TABLE_FMT__BORDER_MIDDLE_RIGHT);
            }
        }
    }

    if (table->config.output_format != TABLE_FMT_OUTPUT_PIPE) {
        table_fmt__print_border_line(table, col_widths, style, TABLE_FMT__BORDER_BOTTOM_LEFT, TABLE_FMT__BORDER_BOTTOM_MIDDLE, TABLE_FMT__BORDER_BOTTOM_RIGHT);
    }
    free(col_widths);
}

static inline void table_fmt__print_space(const TableFmt *table)
{
    if (!table || !table->config.output_stream || table->num_rows == 0) return;

    size_t *col_widths = malloc(sizeof(size_t) * table->config.num_cols);
    if (!col_widths) return;
    table_fmt__calc_col_widths(table, col_widths);

    for (size_t row = 0; row < table->num_rows; ++row) {
        for (size_t col = 0; col < table->config.num_cols; ++col) {
            size_t content_len = table_fmt__content_visible_length(table->row_data[row * table->config.num_cols + col]);
            size_t padding_multiplier = table->config.alignment == TABLE_FMT_ALIGN_CENTRE ? 2 : 1;
            size_t total_cell_width = col_widths[col] + (table->config.cell_padding * padding_multiplier);
            TableFmt__CellPadding cell_padding = table_fmt__get_cell_padding(table, total_cell_width, content_len);
            size_t left_padding = (col == 0 && table->config.alignment == TABLE_FMT_ALIGN_LEFT) ? 0 : cell_padding.left;
            for (size_t i = 0; i < left_padding; ++i) fputc(' ', table->config.output_stream);
            fputs(table->row_data[row * table->config.num_cols + col], table->config.output_stream);
            size_t right_padding = (col == table->config.num_cols - 1 && table->config.alignment == TABLE_FMT_ALIGN_RIGHT) ? 0 : cell_padding.right;
            for (size_t i = 0; i < right_padding; ++i) fputc(' ', table->config.output_stream);
            if (col < table->config.num_cols - 1) fputc(' ', table->config.output_stream);
        }
        fputc('\n', table->config.output_stream);
    }
    free(col_widths);
}

/*
** Public API
*/

static inline TableFmt *table_fmt_init(TableFmtConfig config)
{
    if (config.num_cols <= 0) return NULL;

    TableFmt *table = malloc(sizeof(TableFmt));
    if (!table) return NULL;

    table->row_data_capacity = TABLE_FMT__INIT_SIZE;
    table->row_data_count = 0;
    table->num_rows = 0;
    table->config = config;
    if (!table->config.output_stream) table->config.output_stream = stdout;
    if (table->config.cell_padding == 0) table->config.cell_padding = 1;

    table->row_data = malloc(sizeof(char *) * table->row_data_capacity);
    if (!table->row_data) {
        free(table);
        return NULL;
    }

    return table;
}

/* Creates a new table with mandatory positional argument for cols and optional named arguments */
#define table_fmt_create(...) table_fmt_init((TableFmtConfig){ __VA_ARGS__ })

/* Removes all rows from the table, keeping the configuration */
static inline void table_fmt_clear(TableFmt *table)
{
    if (!table) return;
    table->row_data_count = 0;
    table->num_rows = 0;
}

/* Frees all memory associated with the table. */
static inline void table_fmt_free(TableFmt *table)
{
    if (!table) return;
    if (table->row_data) free(table->row_data);
    free(table);
}

/* Adds a row using variadic arguments. Pass exactly num_cols strings. */
static inline bool table_fmt_row(TableFmt *table, ...)
{
    if (!table) return false;

    if (!table_fmt__realloc(table)) return false;

    va_list args;
    va_start(args, table);
    for (size_t i = 0; i < table->config.num_cols; ++i) {
        const char *value = va_arg(args, char *);
        table->row_data[table->row_data_count++] = value ? value : "";
    }
    table->num_rows++;
    va_end(args);
    return true;
}

/* Adds a row from a string array of length num_cols. */
static inline bool table_fmt_row_array(TableFmt *table, const char **values)
{
    if (!table || !values) return false;

    if (!table_fmt__realloc(table)) return false;

    for (size_t i = 0; i < table->config.num_cols; ++i) {
        table->row_data[table->row_data_count++] = values[i] ? values[i] : "";
    }
    table->num_rows++;
    return true;
}

/* Prints the table to the configured output stream. */
static inline void table_fmt_print(const TableFmt *table)
{
    if (!table) return;

    switch (table->config.output_format) {
    case TABLE_FMT_OUTPUT_BORDERS:
    case TABLE_FMT_OUTPUT_PIPE:
        table_fmt__print_bordered(table);
        break;
    case TABLE_FMT_OUTPUT_SPACE:
        table_fmt__print_space(table);
        break;
    case TABLE_FMT_OUTPUT_CSV:
        table_fmt__print_csv(table);
        break;
    default:
        break;
    }
}

#endif /* TABLE_FMT_H */
