#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef union {
  int int_value;
  float float_value;
  size_t size_value;
  char *string_value;

} answer_t;

typedef bool check_fn(const char *);
typedef answer_t convert_fn(const char *);

extern char *strdup(const char *);

int read_string(char *buf, int buf_siz);

bool is_number(const char *str);
bool is_float(const char *str);
bool is_size(const char *str);
bool not_empty(const char *str);

answer_t make_int(const char *str);
answer_t make_size(const char *str);
answer_t make_float(const char *str);
answer_t make_string(const char *str);

answer_t ask_question(const char *question, check_fn, convert_fn);

int ask_question_int(const char *question);
double ask_question_float(const char *question);
char *ask_question_string(const char *question);
size_t ask_question_size(const char *question);

void print(char *str);
void println(char *str);
