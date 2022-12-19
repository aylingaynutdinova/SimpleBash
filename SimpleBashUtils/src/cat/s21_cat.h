#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define ONE_PAGE 4096

typedef struct {
  int b;
  int e;
  int v;
  int E;
  int n;
  int s;
  int t;
  int T;
} cat_options;

static struct option long_option[] = {{"number", 0, 0, 'n'},
                                      {"squeeze-blank", 0, 0, 's'},
                                      {"number-nonblank", 0, 0, 'b'},
                                      {0, 0, 0, 0}};

void cat_parser(int argc, char *argv[], cat_options *opt);
void n_numberall(int prev_element, int *line_number);
void b_number_nonempty(int prev_element, int *line_number, char n);
void e_print_endline(char c);
void t_tab(char *current);
void s_squeeze(char current, char current_prev);
void v_nonread(char *current);
void just_print(char buffer[]);
void option_realization(int argc, cat_options *opt, char buffer[], int *length);
void reader(int argc, char *argv[], cat_options *opt, char buffer[]);

#endif  // SRC_CAT_S21_CAT_H_