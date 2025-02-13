#ifndef S21_GREP_H
#define S21_GREP_H
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <ctype.h>
#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EOG -1
#define SUCCESS 0
#define BAD_ARGUMENT 1
#define NO_FILE 2
#define BAD_ALLOC 3

static const char SHORT_OPTIONS[] = "e:ivclnhsf:o";
static const struct option LONG_OPTIONS[] = {
    {"regexp", required_argument, NULL, 'e'},
    {"ignore-case", no_argument, NULL, 'i'},
    {"invert-match", no_argument, NULL, 'v'},
    {"count", no_argument, NULL, 'c'},
    {"files-with-matches", no_argument, NULL, 'l'},
    {"line-number", no_argument, NULL, 'n'},
    {"no-filename", no_argument, NULL, 'h'},
    {"no-messages", no_argument, NULL, 's'},
    {"file", required_argument, NULL, 'f'},
    {"only-matching", no_argument, NULL, 'o'}};

typedef struct {
  size_t matches_count;
  size_t files_count;
} Matches;

typedef struct {
  char **data;
  regex_t *reg_data;
  size_t current_size;
  size_t max_size;
} Patterns;

typedef struct {
  int err;
  bool e, i, v, c, l, n, h, s, f, o;
  Matches matches;
  Patterns patterns;
} Options;

void options_init(int argc, char **argv, Options *options);
void patterns_init(Patterns *patterns);
void choose_options(Options *options, char current_option);
void print_error(int err_code, char **file_path);
void *s21_malloc(const size_t size);
void *s21_realloc(void *ptr, const size_t size);
void s21_free(Options *options);

/*option -e*/
void regexp(Options *options, char *str);
void patterns_add(Options *options, char *pattern);

/*option -i*/
void ignore_func(Options *options);

/*+option -s*/
void file_work(int file_count, char **file_path, Options *options);

void s21_grep(FILE *file, char *filename, Options *options);
void grep_output(FILE *file, char *filename, Options *options);

/*+option -v*/
bool find_matches(char *line, regmatch_t *match, Options *options);

/*option -c*/
void count_output(FILE *file, char *filename, Options *options);

/*option -l*/
void files_with_matches_output(FILE *file, char *filename, Options *options);

/*option -n*/
void line_number_output(size_t line_number, Options *options);

/*option -h*/
void filename_output(char *filename, Options *options);

/*option -f*/
void file(Options *options, char *filename);
void buffer_file(FILE *file, char *buffer);

/*option -o*/
void only_matching_output(FILE *file, char *filename, Options *options);

#endif