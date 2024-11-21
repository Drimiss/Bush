#ifndef S21_GREP_H
#define S21_GREP_H

typedef struct arguments {
  int e, i, v, c, l, n, h, s, f, o;
  char *pattern;  //хранит в себе регулярные выражения

  int len_pattern;
  int mem_pattern;

} arguments;
#endif