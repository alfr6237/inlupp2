#pragma once
#include <stdbool.h>
#include <stddef.h>

typedef union elem elem_t;
union elem {
  int i;
  unsigned int u;
  size_t sz;
  bool b;
  float f;
  void *p;
  char *s;
  const char *cs;
  const void *cp;
};

#define int_elem(x) ((elem_t){.i = (x)})
#define uint_elem(x) ((elem_t){.u = (x)})
#define size_elem(x) ((elem_t){.sz = (x)})
#define bool_elem(x) ((elem_t){.b = (x)})
#define float_elem(x) ((elem_t){.f = (x)})
#define ptr_elem(x) ((elem_t){.p = (x)})
#define str_elem(x) ((elem_t){.s = (x)})
#define c_str_elem(x) ((elem_t){.cs = (x)})
#define c_ptr_elem(x) ((elem_t){.cp = (x)})
