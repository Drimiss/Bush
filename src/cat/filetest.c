#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void generate_file(char *file_name);
void generate_lines(FILE *fp);
void generate_spaces(FILE *fp);
void generate_rand(FILE *fp);

int main(int argc, char *argv[]) {
  srand(time(NULL) * atoi(argv[1]));
  for (int i = 2; i < argc; i++) generate_file(argv[i]);
}

void generate_file(char *file_name) {
  FILE *fp;
  fp = fopen(file_name, "w++");
  if (!strstr(file_name, "rand"))
    generate_rand(fp);
  else if (!strcmp(file_name, "empty"))
    ;
  else if (!strcmp(file_name, "lines"))

    generate_rand(fp);
  fclose(fp);
}

void generate_lines(FILE *fp) {
  int n = rand() % 4 + 1;
  for (int i = 0; i < n; i++) fwrite("\n", sizeof(char), 1, fp);
}

void generate_spaces(FILE *fp) {
  char spaces[] = {0x09, 0x0a, 0x0b, 0x0c, 0x0d};
  int n = rand() % 10 + 1;
  for (int i = 0; i < n; i++)
    fwrite(spaces + (rand() % sizeof(spaces)), sizeof(char), 1, fp);
}

void generate_rand(FILE *fp) {
  int n = rand() % 254 + 1;
  if (rand() % 2) generate_lines(fp);
  if (rand() % 2) generate_spaces(fp);
  for (int i = 0; i < n; i++) {
    char c = rand() % 255;
    fwrite(&c, sizeof(char), 1, fp);
  }
  if (rand() % 2) generate_lines(fp);
}
