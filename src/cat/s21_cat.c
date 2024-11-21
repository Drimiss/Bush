#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct arguments {
  int b, n, s, E, T, v;
} arguments;

arguments argument_parser(int argc, char *argv[]) {
  arguments arg = {0};
  if (argc == 1) return arg;
  if (argv[1][0] == '-') {
    struct option long_options[] = {{"number", no_argument, NULL, 'n'},
                                    {"number-nonblank", no_argument, NULL, 'b'},
                                    {"squeeze-blank", no_argument, NULL, 's'},
                                    {0, 0, 0, 0}};
    int opt = 0;
    while ((opt = getopt_long(argc, argv, "bnsEeTtv", long_options, 0)) != -1) {
      switch (opt) {
        case 'b':
          arg.b = 1;
          break;
        case 'n':
          arg.n = 1;
          break;
        case 's':
          arg.s = 1;
          break;
        case 'E':
          arg.E = 1;
          break;
        case 'e':
          arg.E = 1;
          arg.v = 1;
          break;
        case 'T':
          arg.T = 1;
          break;
        case 't':
          arg.T = 1;
          arg.v = 1;
          break;
        case 'v':
          arg.v = 1;
          break;
        case '?':
          exit(1);
        default:
          break;
      }
    }
  }
  return arg;
}

char v_output(char ch) {
  if (ch == '\n' || ch == '\t') return ch;
  if (ch < -96) {
    printf("M-");
    ch = ch & 0x7F;
  }
  if (ch >= 0 && ch <= 31) {
    putchar('^');
    ch += 64;
  } else if (ch == 127) {
    putchar('^');
    ch = '?';
  }
  return ch;
}

void outline(arguments *arg, char *line, int n) {
  for (int i = 0; i < n; i++) {
    if (arg->T && line[i] == '\t')
      printf("^I");
    else {
      if (arg->E && line[i] == '\n') putchar('$');
      if (arg->v) line[i] = v_output(line[i]);
      putchar(line[i]);
    }
  }
}

void output(arguments *arg, FILE *f) {
  char *line = NULL;
  size_t memline = 0;

  int read = 0;
  int line_count = 1;
  int empty_count = 0;
  read = getline(&line, &memline, f);

  while (read != -1) {
    if (line[0] == '\n')
      empty_count++;
    else
      empty_count = 0;
    if (arg->s && empty_count > 1) {
    } else {
      if (arg->b && line[0] != '\n') {
        printf("%6d\t", line_count);
        line_count++;
      } else if (arg->n && !arg->b) {
        printf("%6d\t", line_count);
        line_count++;
      }
      outline(arg, line, read);
    }
    read = getline(&line, &memline, f);
  }
  fclose(f);
  free(line);
}

void read_file(arguments *arg, int argc, char *argv[]) {
  FILE *f;
  if (argc <= optind) {
    f = stdin;
    output(arg, f);
  } else {
    int i = optind ? optind : 1;
    for (; i < argc; i++) {
      f = fopen(argv[i], "r");
      if (f == NULL) {
        fprintf(stderr, "s21_cat: %s: No such file or directory\n", argv[i]);
        exit(1);
      }
      output(arg, f);
    }
  }
}

int main(int argc, char *argv[]) {
  arguments arg = argument_parser(argc, argv);

  read_file(&arg, argc, argv);

  return 0;
}
