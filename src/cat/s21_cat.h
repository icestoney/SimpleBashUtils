#ifndef S21_CAT_H
#define S21_CAT_H
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define _GNU_SOURCE
#define BAD_FILE 2

typedef struct {
  bool b, n, s, v, E, T;
} Options;

void choose_options(int argc, char **argv, Options *options);
void file_work(int files_count, char *const *file_path,
               const Options *const options);
void print_error(int code, char *const *file_name);
void s21_cat(FILE *file, const Options *const options);
void nl_counter(unsigned char current_symbol, unsigned int *new_line_counter);
void print_line_number(unsigned char current_symbol,
                       unsigned char previous_symbol,
                       const Options *const options);
void print_end_line_symbol(unsigned char current_symbol);
void symbol_print(unsigned char current_symbol, const Options *const options);

#endif