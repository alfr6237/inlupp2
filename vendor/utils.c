#include "utils.h"

// #include <_inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool not_empty(char *str) { return strlen(str) > 0; }

void clear_input_buffer(void) {
  int c;
  do {
    c = getchar();
  } while (c != '\n' && c != EOF && c != '\0');
}

int read_string(char *buf, int buf_siz) {
  int i = 0;
  while (i < buf_siz - 1) {
    char c = getchar();
    if (c == '\n') {
      break;
    }
    buf[i] = c;
    i++;
  }

  buf[i] = '\0';
  return i;
}

answer_t ask_question(char *question, check_fn *check, convert_fn *convert) {

  int buffer_size = 255;
  char buffer[buffer_size];

  do {
    printf("%s\n", question);
    read_string(buffer, buffer_size);
    if (check(buffer)) {
      return convert(buffer);
    }
    printf("Try again\n");
  } while (true);
}

int ask_question_int(char *question) {
  answer_t answer = ask_question(question, is_number, make_int);
  return answer.int_value;
}

char *ask_question_string(char *question) {
  answer_t answer = ask_question(question, not_empty, make_string);
  return answer.string_value;
}

double ask_question_float(char *question) {
  return ask_question(question, is_float, make_float).float_value;
}

answer_t make_int(char *str) { return (answer_t){.int_value = atoi(str)}; }
answer_t make_float(char *str) { return (answer_t){.float_value = atof(str)}; }
answer_t make_string(char *str) {
  return (answer_t){.string_value = strdup(str)};
}

bool is_float(char *str) {

  int len = strlen(str);
  if (!len) {
    return false;
  }

  int start = str[0] == '-' ? 1 : 0;

  bool decimal_point_found = false;
  for (int i = start; i < len; i++) {
    if (str[i] == '.') {
      if (decimal_point_found) {
        return false;
      } else {
        decimal_point_found = true;
      }
    } else if (str[i] < '0' || str[i] > '9') {
      return false;
    }
  }

  return true;
}
bool is_number(char *str) {
  int len = strlen(str);
  if (!len) {
    return false;
  }

  int start = str[0] == '-' ? 1 : 0;

  for (int i = start; i < len; i++) {
    if (str[i] < '0' || str[i] > '9') {
      return false;
    }
  }

  return true;
}

void print(char *str) {
  int len = strlen(str);
  for (int i = 0; i < len; i++) {
    putchar(str[i]);
  }
}

void println(char *str) {
  print(str);
  putchar('\n');
}
