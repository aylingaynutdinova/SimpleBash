#include "s21_grep.h"

// check leaks

void grep_parser(int argc, char *argv[], char arguments[M][M],
                 grep_options *flags, int *argumemt_index, int *count) {
  int options = 0;
  int option_index = 0;
  char buffer_f[BUFSIZE] = {0};
  while ((options = getopt_long(argc, argv, "e:ivclnhsf:o", 0,
                                &option_index)) != -1) {
    switch (options) {
      case 'e':
        flags->e = 1;
        strcpy(arguments[*count], optarg);
        *count = *count + 1;
        break;

      case 'n':
        flags->n = 1;
        break;

      case 'v':
        flags->v = 1;
        break;

      case 'i':
        flags->i = 1;
        break;

      case 'f':
        flags->f = 1;
        strcpy(arguments[*count], optarg);
        *count = *count + 1;
        break;

      case 'c':
        flags->c = 1;
        break;

      case 'l':
        flags->l = 1;
        break;

      case 'h':
        flags->h = 1;
        break;

      case 's':
        flags->s = 1;
        break;

      case 'o':
        flags->o = 1;
        break;
    }
  }
  if (optarg != 0) {
    strcpy(arguments[*count], optarg);
    *count = *count + 1;
  }
  if ((flags->f == 0) && (flags->e == 0)) {
    strcpy(arguments[*count], argv[optind]);
    *count = *count + 1;
    *argumemt_index = *argumemt_index + 1;
  }
  if (flags->f == 1) {
    FILE *fp;
    fp = fopen(arguments[*count - 1], "r");
    if (fp) {
      while (fgets(buffer_f, BUFSIZE, fp)) {
        strcpy(arguments[*count], buffer_f);
        *count = *count + 1;
      }
      fclose(fp);
    } else {
      fprintf(stderr, "No such file or directory\n");
    }
  }
}

void reader(int argc, char *argv[], char buffer[], grep_options *flags,
            char arguments[M][M], int *count) {
  (void)arguments;
  (void)flags;
  (void)buffer;
  int line_number = 0;
  int line_matchs = 0;
  FILE *fp = NULL;
  int cflag = 0;
  int file_amount = argc - optind;
  if (flags->i == 1) {
    cflag = REG_ICASE;
  } else {
    cflag = REG_EXTENDED;
  }
  int file_counter = 0;
  if ((flags->e != 1) && (flags->f != 1)) {
    file_counter = 1;
    file_amount = argc - optind - 1;
  }
  while (argv[optind + file_counter] != NULL) {
    fp = fopen(argv[optind + file_counter], "r");
    if (fp) {
      for (int j = 0; j < *count; j++) {
        option_realization(argv, flags, arguments[j], buffer, &cflag,
                           &line_number, file_amount, file_counter, fp,
                           &line_matchs);
        rewind(fp);
        line_number = 0;
      }
      fclose(fp);
      other_flags(argv, flags, file_counter, &line_matchs, file_amount);
      l_realization(argv, flags, file_counter, &line_matchs, file_amount);
      line_number = 0;
      line_matchs = 0;
      // fclose(fp);
    } else {
      if (flags->s == 0) {
        fprintf(stderr, "No such file or directory\n");
      }
    }
    file_counter++;
  }
}

int match_func(char *buffer, char *arguments, regex_t *re, int cflag,
               regmatch_t *pmatch) {
  int status = 0;
  if ((status = regcomp(re, arguments, cflag)) != 0) {
    regfree(re);
    return (status);
  }
  status = regexec(re, buffer, 1, pmatch, 0);
  // regfree(re);
  return (status);
}

void o_realization(char *buffer, regmatch_t regmatch[1], regex_t re,
                   int retval) {
  char *ptr = buffer;
  while (!retval && regmatch[0].rm_eo != regmatch[0].rm_so) {
    printf("%.*s\n", (int)(regmatch[0].rm_eo - regmatch[0].rm_so),
           ptr + regmatch[0].rm_so);
    ptr += regmatch[0].rm_eo;
    retval = regexec(&re, ptr, 1, regmatch, REG_NOTBOL);
  }
}
void option_realization(char *argv[], grep_options *flags, char *arguments,
                        char *buffer, int *cflag, int *line_number,
                        int file_amount, int file_counter, FILE *fp,
                        int *line_matchs) {
  int retval = 0;
  regex_t re;
  regmatch_t regmatch[1];
  if ((flags->c) || (flags->l) || (flags->v)) flags->o = 0;
  while (fgets(buffer, BUFSIZE, fp)) {
    int match = 0;
    *line_number = *line_number + 1;
    retval = match_func(buffer, arguments, &re, *cflag, regmatch);
    if ((retval == 0 && !flags->v) || (retval == REG_NOMATCH && flags->v)) {
      match = 1;
    }

    if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = '\0';

    if ((flags->c != 1) && (flags->l != 1)) {
      if (((retval == 0) && (flags->v != 1)) ||
          ((retval != 0) && (flags->v == 1))) {
        if ((flags->h != 1) && (file_amount > 1)) {
          printf("%s:", argv[optind + file_counter]);
        }
        if ((flags->n == 1)) {
          printf("%d:", *line_number);
        }
        if (!flags->o) {
          printf("%s\n", buffer);
        }
      }
    }
    if (flags->o && !flags->v) {
      o_realization(buffer, regmatch, re, retval);
      regfree(&re);
    } else {
      regfree(&re);
    }
    *line_matchs = *line_matchs + match;
    if (buffer[0] == '\n') *line_matchs = *line_matchs - 1;
  }
}

void other_flags(char *argv[], grep_options *flags, int file_counter,
                 int *line_matchs, int file_amount) {
  if (flags->c == 1) {
    if (file_amount > 1) {
      if ((flags->l != 1) && (flags->v == 0) && (!flags->h)) {
        printf("%s:%d\n", argv[optind + file_counter], *line_matchs);
      }
      if ((flags->v == 1)) {
        printf("%s:%d\n", argv[optind + file_counter], *line_matchs);
      } else if (flags->h)
        printf("%d\n", *line_matchs);
    } else {
      if (((flags->v == 0) || (flags->v == 1)) && (flags->l != 1))
        printf("%d\n", *line_matchs);
    }
  }
}

void l_realization(char *argv[], grep_options *flags, int file_counter,
                   int *line_matchs, int file_amount) {
  int l_number = 0;
  if ((flags->c == 1) && (flags->l == 1)) {
    if (file_amount == 1) {
      if ((*line_matchs == 0)) printf("%d", l_number);
      if (*line_matchs > 0) {
        l_number++;
        printf("%d\n", l_number);
        printf("%s\n", argv[optind + file_counter]);
      }
    }
    if (file_amount > 1) {
      if (*line_matchs == 0)
        printf("%s:%d\n", argv[optind + file_counter], l_number);
      if (*line_matchs > 0) {
        l_number++;
        if (!flags->c) {
          printf("%s:%d\n", argv[optind + file_counter], l_number);
          printf("%s\n", argv[optind + file_counter]);
        } else if (flags->c) {
          printf("%d\n", l_number);
          printf("%s\n", argv[optind + file_counter]);
        }
      }
    }
  }
  if ((flags->l == 1) && (*line_matchs > 0) && (flags->c != 1))
    printf("%s\n", argv[optind + file_counter]);
}

void initialization(int argc, char *argv[]) {
  int count = 0;
  int argument_index = 0;
  grep_options flags = {0};
  char arguments[M][M] = {0};
  char buffer[BUFSIZE] = {0};
  if ((argv[optind] != NULL) && (argc > 3)) {
    grep_parser(argc, argv, arguments, &flags, &argument_index, &count);
    reader(argc, argv, buffer, &flags, arguments, &count);
  }
}

int main(int argc, char *argv[]) {
  initialization(argc, argv);
  return 0;
}
