#include "s21_cat.h"

int main(int argc, char **argv) {
  Options options = {0};
  if (argc >= 2) {
    choose_options(argc, argv, &options);
    file_work(argc - optind, argv + optind, &options);
  }
  return 0;
}

void choose_options(int argc, char **argv, Options *options) {
  const char *SHORT_OPTIONS = "AbeEnstTv";
  const struct option LONG_OPTIONS[] = {
      {"show-all", 0, NULL, 'A'},      {"number-nonblank", 0, NULL, 'b'},
      {"number", 0, NULL, 'n'},        {"show-ends", 0, NULL, 'E'},
      {"squeeze-blank", 0, NULL, 's'}, {"show-tabs", 0, NULL, 'T'}};
  int option = 0, longind = 0;
  while ((option = getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS,
                               &longind)) != -1) {
    switch (option) {
      case 'A':
        options->v = true;
        options->E = true;
        options->T = true;
        break;
      case 'b':
        options->b = true;
        break;
      case 'e':
        options->v = true;
        options->E = true;
        break;
      case 'n':
        options->n = true;
        break;
      case 's':
        options->s = true;
        break;
      case 't':
        options->v = true;
        options->T = true;
        break;
      case 'v':
        options->v = true;
        break;
      case 'E':
        options->E = true;
        break;
      case 'T':
        options->T = true;
        break;
      default:
        break;
    }
  }
  if (options->b == true) {
    options->n = false;
  }
}

void file_work(int files_count, char *const *file_path,
               const Options *const options) {
  FILE *file = NULL;
  while (files_count > 0) {
    file = fopen(*file_path, "r");
    if (file == NULL) {
      print_error(BAD_FILE, file_path);
    } else {
      s21_cat(file, options);
      fclose(file);
    }
    fflush(stdout);
    ++file_path;
    --files_count;
  }
}

void s21_cat(FILE *file, const Options *const options) {
  unsigned char current_symbol = fgetc(file);
  static unsigned char previous_symbol = '\n';
  static unsigned int new_line_counter = 1;
  while (!feof(file)) {
    if (options->s) {  // -s
      nl_counter(current_symbol, &new_line_counter);
    }
    if (current_symbol != '\n' || new_line_counter <= 2) {
      if (options->b || options->n) {  // -b || -n
        print_line_number(current_symbol, previous_symbol, options);
      }
      if (options->E) {  // -e (vE) || -E
        print_end_line_symbol(current_symbol);
      }
      symbol_print(current_symbol, options);
    }
    previous_symbol = current_symbol;
    current_symbol = fgetc(file);
  }
}

void print_line_number(unsigned char current_symbol,
                       unsigned char previous_symbol,
                       const Options *const options) {
  static unsigned line_counter = 0;

  if (current_symbol != '\n' && previous_symbol == '\n' && options->b) {
    fprintf(stdout, "%6u\t", ++line_counter);
  } else if (previous_symbol == '\n' && options->n) {
    fprintf(stdout, "%6u\t", ++line_counter);
  }
}

void print_end_line_symbol(unsigned char current_symbol) {
  if (current_symbol == '\n') {
    fputc('$', stdout);
  }
}

void symbol_print(unsigned char current_symbol, const Options *const options) {
  int shift = 0;
  if (!options->v && options->T && current_symbol == '\t') {  // -T
    shift = 64;
    fputc('^', stdout);
  }
  if (options->v && options->T && current_symbol == '\t') {  // -t
    shift = 64;
    fputc('^', stdout);
  }
  if (options->v) {  // -v
    if (current_symbol > 127 && current_symbol < 160) printf("M-^");
    if ((current_symbol < 32 && (current_symbol != '\t') &&
         (current_symbol != '\n')) ||
        (current_symbol == 127))
      printf("^");
    if ((current_symbol < 32 ||
         (current_symbol > 126 && current_symbol < 160)) &&
        current_symbol != '\n' && current_symbol != '\t') {
      current_symbol = current_symbol > 126 ? current_symbol - 128 + 64
                                            : current_symbol + 64;
    }
    if (current_symbol > 159 && current_symbol < 255) {
      printf("M-");
      current_symbol -= 128;
    }
    if (current_symbol == 255) {
      printf("M-^");
      current_symbol = '?';
    }
  }
  fputc(current_symbol + shift, stdout);
}

void nl_counter(unsigned char current_symbol, unsigned int *new_line_counter) {
  if (current_symbol == '\n') {
    *new_line_counter += 1;
  } else {
    *new_line_counter = 0;
  }
}

void print_error(int code, char *const *file_name) {
  switch (code) {
    case BAD_FILE:
      fprintf(stderr, "cat: %s: No such file or directory\n", *file_name);
    default:
      break;
  }
}