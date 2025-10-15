#ifndef TABLE_H
#define TABLE_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define TABLE_INIT_SIZE 64

typedef enum {
    FORMAT_CSV = 0,     // Comma-separated values
    FORMAT_TABLE = 1    // ASCII table
} OutputFormat;

typedef struct {
    FILE *output_stream;
    OutputFormat output_format;
    unsigned int rows_buffer_size;
    unsigned char ***rows_buffer;
    unsigned int num_rows;
    unsigned int num_cols;
} Table;

static inline void free_table(Table *table)
{
    if (table) {
        if (table->rows_buffer) {
            for (unsigned int i = 0; i < table->num_rows; i++) {
                free(table->rows_buffer[i]);
            }
            free(table->rows_buffer);
        }
        free(table);
    }
}

static inline Table *table_init(FILE *output_stream, OutputFormat output_format, unsigned int num_cols)
{
    Table *table = malloc(sizeof(Table));
    if (table == NULL) {
        fprintf(stderr, "Failed to malloc Table");
        return NULL;
    }

    table->rows_buffer_size = TABLE_INIT_SIZE;
    table->rows_buffer = malloc(sizeof(unsigned char **) * TABLE_INIT_SIZE);
    if (table->rows_buffer == NULL) {
        fprintf(stderr, "Failed to malloc Table row_buffer");
        free(table);
        return NULL;
    }

    table->num_rows = 0;
    table->num_cols = num_cols;
    table->output_stream = output_stream;
    table->output_format = output_format;

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

    unsigned char **new_row = malloc(sizeof(unsigned char *) * table->num_cols);
    if (new_row == NULL) {
        fprintf(stderr, "Failed to allocate row\n");
        va_end(args);
        return;
    }

    for (unsigned int i = 0; i < table->num_cols; i++) {
        new_row[i] = va_arg(args, unsigned char *);
    }

    table->rows_buffer[table->num_rows] = new_row;
    table->num_rows++;

    va_end(args);
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

static inline void table_print_ascii(const Table *table)
{
    if (!table || !table->output_stream) return;
    fprintf(table->output_stream, "TODO: ASCII table not yet implemented\n");
}

static inline void table_print(const Table *table)
{
    if (!table) return;

    switch (table->output_format) {
    case FORMAT_CSV:
        table_print_csv(table);
        break;
    case FORMAT_TABLE:
        table_print_ascii(table);
        break;
    }
}

#endif // TABLE_H
