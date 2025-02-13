#include "s21_grep.h"

int main(int argc, char **argv) {
  Options options = {0};
  if (argc <= 1) exit(BAD_ARGUMENT);
  options_init(argc, argv, &options);
  if (options.err != SUCCESS) {
    s21_free(&options);
    exit(options.err);
  }
  file_work(argc - optind, argv + optind, &options);
  s21_free(&options);
  return 0;
}

void options_init(int argc, char **argv, Options *options) {
  options->err = SUCCESS;
  patterns_init(&options->patterns);
  int longind = 0;
  char current_option =
      getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS, &longind);

  while (current_option != EOG) {
    choose_options(options, current_option);
    current_option =
        getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS, &longind);
  }
  if (options->err == SUCCESS) {
    if (!options->e && !options->f) {
      regexp(options, argv[optind++]);
    }
    ignore_func(options);
    options->matches.files_count = argc - optind;
  } else {
    return;
  }
}

void patterns_init(Patterns *patterns) {
  patterns->current_size = 0;
  patterns->max_size = 128;
  patterns->data = s21_malloc(patterns->max_size * sizeof(char *));
  if (patterns->data == NULL) {
    fprintf(stderr, "[DEBUG] Bad memory allocation.\n");
  }
}

void choose_options(Options *options, char current_option) {
  switch (current_option) {
    case 'e':
      options->e = true;
      regexp(options, optarg);
      break;

    case 'i':
      options->i = true;
      break;

    case 'v':
      options->v = true;
      break;

    case 'c':
      options->c = true;
      break;

    case 'l':
      options->l = true;
      break;

    case 'n':
      options->n = true;
      break;

    case 'h':
      options->h = true;
      break;

    case 's':
      options->s = true;
      break;

    case 'f':
      options->f = true;
      file(options, optarg);
      break;

    case 'o':
      options->o = true;
      break;

    default:
      print_error(BAD_ARGUMENT, NULL);
      break;
  }
}

/*option -e*/
void regexp(Options *options, char *str) {
  char *temp_patterns = s21_malloc(sizeof(char) * strlen(str) + sizeof(char));
  strcpy(temp_patterns, str);
  char *token = strtok(temp_patterns, "\n");
  while (token != NULL) {
    patterns_add(options, token);
    token = strtok(NULL, "\n");
  }
  free(temp_patterns);
}

void patterns_add(Options *options, char *pattern) {
  Patterns *patterns = &options->patterns;
  if (patterns->current_size == patterns->max_size) {
    patterns->max_size += 128;
    patterns->data =
        s21_realloc(patterns->data, patterns->max_size * sizeof(char *));
  }
  patterns->data[patterns->current_size] =
      s21_malloc(sizeof(char) * strlen(pattern) + sizeof(char));
  strcpy(patterns->data[patterns->current_size++], pattern);
}

/*option -f*/
void file(Options *options, char *filename) {
  FILE *file = fopen(filename, "r");
  if (file != NULL) {
    char *buffer = s21_malloc(sizeof(char) * 128);
    buffer_file(file, buffer);
    regexp(options, buffer);
    free(buffer);
    fclose(file);
  } else {
    print_error(NO_FILE, &filename);
    options->err = NO_FILE;
  }
}

void buffer_file(FILE *file, char *buffer) {
  size_t size = 0;
  size_t max_size = 128;
  char symbol = fgetc(file);
  while (!feof(file)) {
    buffer[size++] = symbol;
    symbol = fgetc(file);
    if (size == max_size) {
      max_size *= 2;
      buffer = s21_realloc(buffer, max_size * sizeof(char));
    }
  }
  buffer[size] = '\0';
}

/*option -i*/
void ignore_func(Options *options) {
  options->patterns.reg_data =
      s21_malloc(options->patterns.current_size * sizeof(regex_t));
  int patterns_case = 0;
  if (options->i) {
    patterns_case = REG_ICASE;
  }
  for (size_t i = 0; i < options->patterns.current_size; i++) {
    regcomp(&options->patterns.reg_data[i], options->patterns.data[i],
            patterns_case);
  }
}

/*+option -s*/
void file_work(int file_count, char **file_path, Options *options) {
  for (FILE *file = NULL; file_count--; ++file_path) {
    file = fopen(*file_path, "r");
    if (file != NULL) {
      s21_grep(file, *file_path, options);
      fflush(stdout);
      fclose(file);
    } else if (!options->s) {
      print_error(NO_FILE, file_path);
    }
  }
}

void s21_grep(FILE *file, char *filename, Options *options) {
  if (options->l) {
    files_with_matches_output(file, filename, options);
  } else if (options->c) {
    count_output(file, filename, options);
  } else if (options->o) {
    only_matching_output(file, filename, options);
  } else {
    grep_output(file, filename, options);
  }
}

void grep_output(FILE *file, char *filename, Options *options) {
  char *line = NULL;
  size_t line_size = 0, line_number = 0;
  while (getline(&line, &line_size, file) != -1) {
    line_number++;
    if (find_matches(line, NULL, options)) {
      filename_output(filename, options);
      line_number_output(line_number, options);
      if (line[strlen(line) - 1] == '\n') {
        fprintf(stdout, "%s", line);
      } else {
        fprintf(stdout, "%s\n", line);
      }
    }
  }
  free(line);
}

/*+option -v*/
bool find_matches(char *line, regmatch_t *match, Options *options) {
  bool result = false;
  size_t nmatch = 0;
  if (match) {
    nmatch = 1;
  }
  for (size_t i = 0; i < options->patterns.current_size; ++i) {
    if (regexec(&options->patterns.reg_data[i], line, nmatch, match, 0) == 0) {
      result = true;
    }
  }

  if (options->v) {
    result = !result;
    if (options->o) {
      result = false;
    }
  }
  return result;
}

/*option -l*/
void files_with_matches_output(FILE *file, char *filename, Options *options) {
  char *line = NULL;
  size_t line_size = 0;
  while (getline(&line, &line_size, file) != EOF) {
    if (find_matches(line, NULL, options)) {
      fprintf(stdout, "%s\n", filename);
      break;
    }
  }
  free(line);
}

/*option -c*/
void count_output(FILE *file, char *filename, Options *options) {
  char *line = NULL;
  size_t line_size = 0;
  unsigned int matches_count = 0;
  while (getline(&line, &line_size, file) != EOF) {
    if (find_matches(line, NULL, options)) {
      matches_count++;
    }
  }
  filename_output(filename, options);
  fprintf(stdout, "%u\n", matches_count);
  free(line);
}

/*option -o*/
void only_matching_output(FILE *file, char *filename, Options *options) {
  char *line = NULL;
  regmatch_t match = {0};
  size_t line_size = 0;
  size_t line_number = 0;
  while (getline(&line, &line_size, file) != -1) {
    char *line_ptr = line;
    ++line_number;
    while (find_matches(line_ptr, &match, options)) {
      filename_output(filename, options);
      line_number_output(line_number, options);
      fprintf(stdout, "%.*s\n", (int)(match.rm_eo - match.rm_so),
              (line_ptr + match.rm_so));
      line_ptr += match.rm_eo;
    }
  }
  free(line);
}

/*option -h*/
void filename_output(char *filename, Options *options) {
  if (options->matches.files_count > 1 && !options->h) {
    fprintf(stdout, "%s:", filename);
  }
}

/*option -n*/
void line_number_output(size_t line_number, Options *options) {
  if (options->n) {
    fprintf(stdout, "%zu:", line_number);
  }
}

void print_error(int err_code, char **file_path) {
  switch (err_code) {
    case BAD_ARGUMENT:
      fprintf(stderr, "Usage: ./s21_grep [OPTION]... PATTERNS [FILE]...\n");
      exit(BAD_ARGUMENT);
      break;

    case NO_FILE:
      fprintf(stderr, "s21_grep: %s: No such file or directory\n", *file_path);
      break;

    default:
      break;
  }
}

void *s21_malloc(const size_t size) {
  void *malloc_ptr = malloc(size);
  if (malloc_ptr == NULL) {
    fprintf(stderr, "%zu[DEBUG] Bad memory allocation\n", size);
    exit(BAD_ALLOC);
  }
  return malloc_ptr;
}

void *s21_realloc(void *ptr, const size_t size) {
  void *realloc_ptr = realloc(ptr, size);
  if (realloc_ptr == NULL) {
    fprintf(stderr, "%zu[DEBUG] Bad memory reallocation\n", size);
    exit(BAD_ALLOC);
  }
  return realloc_ptr;
}

void s21_free(Options *options) {
  for (size_t i = 0; i < options->patterns.current_size; ++i) {
    free(options->patterns.data[i]);
    regfree(&options->patterns.reg_data[i]);
  }
  free(options->patterns.data);
  free(options->patterns.reg_data);
}
