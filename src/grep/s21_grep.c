// проверь флаги -on и -ei pattern

#include "s21_grep.h"

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void pattern_add(arguments *arg, char *pattern) {
  if (pattern == NULL) {
    return;
  }
  int n = strlen(pattern);
  if (arg->len_pattern == 0) {
    arg->pattern = malloc(1024 * sizeof(char));
    arg->pattern[0] = '\0';
    arg->mem_pattern = 1024;
  }
  if (arg->mem_pattern < arg->len_pattern + n) {
    arg->pattern = realloc(arg->pattern, arg->mem_pattern * 2);
  }
  if (arg->len_pattern != 0) {
    strcat(arg->pattern + arg->len_pattern, "|");
    arg->len_pattern++;
  }
  arg->len_pattern += sprintf(arg->pattern + arg->len_pattern, "(%s)", pattern);
}

void add_reg_from_file(arguments *arg, char *filepath) {
  FILE *f = fopen(filepath, "r");
  if (f == NULL) {
    if (!arg->s) perror(filepath);
  }
  char *line = NULL;
  size_t memlen = 0;
  int read = getline(&line, &memlen, f);
  while (read != -1) {
    if (line[read - 1] == '\n') line[read - 1] = '\0';
    pattern_add(arg, line);
    read = getline(&line, &memlen, f);
  }
  free(line);
  fclose(f);
}

arguments argument_parser(int argc, char *argv[]) {
  arguments arg = {0};
  int opt = 0;
  while ((opt = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) != -1) {
    switch (opt) {
      case 'e': /*Шаблон начинающийся с '-'*/
        arg.e = 1;
        pattern_add(&arg, optarg); /*в optarg сохраняем тут наш regex*/
        break;
      case 'i':
        arg.i = REG_ICASE; /*flag -i, вывод данных вне зависимости от регистра
                              (регистр независимого поиска)*/
        break;
      case 'v': /*Выдает все строки, за исключением содержащих образец.*/
        arg.v = 1;
        break;
      case 'c': /*Выдает только количество строк, содержащих образец.*/
        arg.c = 1;
        break;
      case 'l': /*Выдает только имена файлов, содержащих сопоставившиеся строки,
                  по одному в строке. Если образец найден в нескольких строках
                  файла, имя файла не повторяется.*/
        arg.c = 1;
        arg.l = 1;
        break;
      case 'n':
        arg.n = 1; /*Выдает перед каждой строкой ее номер в файле (строки
                      нумеруются с 1).*/
        break;
      case 'h': /*Выводит совпадающие строки, не предваряя их именами файлов.*/
        arg.h = 1;
        break;
      case 's': /*Подавляет выдачу сообщений о не существующих или недоступных
                   для чтения файлах.*/
        arg.s = 1;
        break;
      case 'f': /*Получает регулярные выражения из файла.*/
        arg.f = 1;
        add_reg_from_file(&arg, optarg);
        break;
      case 'o':
        arg.o = 1; /*Печатает только совпадающие (непустые) части совпавшей
                      строки.*/
        break;
      default:
        printf(
            "usage: s21_grep [-cefhilnosv] [-e pattern] [-f file] [--null] "
            "[pattern] [file ...]");
        exit(1);
        // }
    }
  }
  if (arg.len_pattern == 0) {
    pattern_add(&arg, argv[optind]);
    optind++;
  }
  if (argc - optind == 1) {
    arg.h = 1;
  }
  return arg;
}

void output_line(char *line, int n) {
  for (int i = 0; i < n; i++) {
    putchar(line[i]);
  }
  if (line[n - 1] != '\n') putchar('\n');
}

void print_match(regex_t *re, char *line) {
  regmatch_t match;
  int offset = 0;

  while (1) {
    int result = regexec(re, line + offset, 1, &match, 0);

    if (result != 0) break;

    for (int i = match.rm_so; i < match.rm_eo; i++) {
      putchar(line[offset + i]);
    }
    putchar('\n');

    offset += match.rm_eo;
  }
}

void processFile(arguments arg, char *path, regex_t *reg) {
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    if (!arg.s) perror(path);
    return;
  }
  char *line = NULL;
  size_t memlen = 0;
  int read = 0;
  int line_count = 1;
  int c = 0;

  read = getline(&line, &memlen, f);
  while (read != -1) {
    int result = regexec(reg, line, 0, NULL, 0);
    if ((result == 0 && !arg.v) || (arg.v && result != 0)) {
      if (!arg.c && !arg.l) {
        if (!arg.h) printf("%s:", path);
        if (arg.n) printf("%d:", line_count);
        if (arg.o) {
          print_match(reg, line);
        } else {
          output_line(line, read);
        }
      }
      c++;
    }
    read = getline(&line, &memlen, f);
    line_count++;
  }
  free(line);
  if (arg.c && !arg.l) {
    if (!arg.h) printf("%s:", path);
    printf("%d\n", c);
  }
  if (arg.l && c > 0) printf("%s\n", path), fclose(f);
}

void output(arguments arg, int argc, char **argv) {
  regex_t re;
  int error = regcomp(&re, arg.pattern, REG_EXTENDED | arg.i);
  if (error) perror("Error");
  for (int i = optind; i < argc; i++) {
    processFile(arg, argv[i], &re);
  }
  regfree(&re);
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    arguments arg = argument_parser(argc, argv);
    output(arg, argc, argv);
    free(arg.pattern);
  } else
    printf(
        "usage: s21_grep [-cefhilnosv] [-e pattern] [-f file] [--null] "
        "[pattern] [file ...]");
  return 0;
}