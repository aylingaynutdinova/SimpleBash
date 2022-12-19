#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

#include <fcntl.h>
#include <getopt.h>
#include <locale.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ONE_PAGE 4096
#define BUFSIZE 256
#define M 10

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
} grep_options;

void grep_parser(int argc, char *argv[], char arguments[M][M],
                 grep_options *flags, int *argumemt_index, int *count);
int match_func(char *buffer, char *arguments, regex_t *re, int cflag,
               regmatch_t *pmatch);
void o_realization(char *buffer, regmatch_t regmatch[1], regex_t re,
                   int retval);
void option_realization(char *argv[], grep_options *flags, char *arguments,
                        char *buffer, int *cflag, int *line_number,
                        int file_amount, int file_counter, FILE *fp,
                        int *line_matchs);
void other_flags(char *argv[], grep_options *flags, int file_counter,
                 int *line_matchs, int file_amount);
void l_realization(char *argv[], grep_options *flags, int file_counter,
                   int *line_matchs, int file_amount);
void reader(int argc, char *argv[], char buffer[], grep_options *flags,
            char arguments[M][M], int *count);
void initialization(int argc, char *argv[]);

#endif  // SRC_GREP_S21_GREP_H_
