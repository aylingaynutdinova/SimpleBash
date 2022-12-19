#include "s21_cat.h"

void cat_parser(int argc, char *argv[], cat_options *opt) {
  int options = 0;
  int option_index = 0;
  while ((options = getopt_long(argc, argv, "+bevnstET", long_option,
                                &option_index)) != -1) {
    switch (options) {
      case 'b':
        opt->b = 1;
        break;
      case 'e':
        opt->e = 1;
        opt->v = 1;
        break;
      case 'v':
        opt->v = 1;
        break;
      case 'E':
        opt->E = 1;
        break;
      case 'n':
        opt->n = 1;
        break;
      case 's':
        opt->s = 1;
        break;
      case 't':
        opt->t = 1;
        opt->v = 1;
        break;
      case 'T':
        opt->T = 1;
        break;
    }
  }
}

void n_numberall(int prev_element, int *line_number) {
  if (prev_element == '\n') {
    printf("%6d\t", *line_number);
    *line_number = *line_number + 1;
  }
}

void b_number_nonempty(int prev_element, int *line_number, char n) {
  if (prev_element == '\n' && n != '\n') {
    printf("%6d\t", *line_number);
    *line_number = *line_number + 1;
  }
  prev_element = n;
}

void e_print_endline(char c) {
  if (c == '\n') {
    printf("$");
  }
}

void t_tab(char *current) {
  if (*current == '\t') {
    printf("^");
    *current = 'I';
  }
}

void s_squeeze(char current, char current_prev) {
  if (current == '\n' && current_prev == '\n') {
    printf("%c", current_prev);
  }
}

void v_nonread(char *current) {
  if ((*current >= 0 && *current <= 8) || (*current >= 11 && *current <= 31)) {
    printf("^");
    *current = *current + 64;
  }
  if (*current == 127) {
    printf("^");
    *current = *current - 64;
  }
}

void just_print(char buffer[]) {
  int length = strlen(buffer);
  for (int i = 0; i < length; i++) {
    printf("%c", buffer[i]);
  }
}

void option_realization(int argc, cat_options *opt, char buffer[],
                        int *length) {
  (void)argc;
  int line_number = 1;
  char prev_element = '\n';

  int empty_str_count = 0;
  int last_sym = '\n';

  for (int i = 0; i < *length; i++) {
    if (opt->s && buffer[i] == '\n' && last_sym == '\n') {
      empty_str_count++;
      if (empty_str_count > 1) {
        continue;
      }
    } else {
      empty_str_count = 0;
    }
    if (opt->n == 1 && opt->b == 0) {
      n_numberall(prev_element, &line_number);
    }
    if (opt->b == 1) {
      b_number_nonempty(prev_element, &line_number, buffer[i]);
    }
    if (opt->e == 1) {
      e_print_endline(buffer[i]);
    }
    if (opt->t == 1) {
      t_tab(&buffer[i]);
    }
    if (opt->v == 1) {
      v_nonread(&buffer[i]);
    }
    printf("%c", buffer[i]);
    last_sym = buffer[i];
    prev_element = buffer[i];
  }
}

void reader(int argc, char *argv[], cat_options *opt, char buffer[]) {
  (void)opt;
  int fd = 0;
  for (int i = optind; i < argc; i++) {
    fd = open(argv[i], O_RDONLY);
    int bytes_read;
    bytes_read = read(fd, buffer, ONE_PAGE);
    // read(fd, buffer, ONE_PAGE);
    option_realization(argc, opt, buffer, &bytes_read);
  }
}

int main(int argc, char *argv[]) {
  cat_options opt = {0};
  char buffer[ONE_PAGE] = {0};
  cat_parser(argc, argv, &opt);
  reader(argc, argv, &opt, buffer);
  return 0;
}
