#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#define MEMORY_SIZE 59049
#define WIDTH 10
#define MAGIC 94
#define JMP 4
#define OUT 5
#define IN 23
#define ROTR 39
#define MOV 40
#define CRZ 62
#define NOP 68
#define HALT 81
int tbl[MAGIC] = {
    57, 109, 60,  46,  84,  86,  97,  99,  96,  117,
    89, 42,  77,  75,  39,  88,  126, 120, 68,  108,
    125, 82,  69,  111, 107, 78,  58,  35,  63,  71,
    34,  105, 64,  53,  122, 93,  38,  103, 113, 116,
    121, 102, 114, 36,  40,  119, 101, 52,  123, 87,
    80,  41,  72,  45,  90,  110, 44,  91,  37,  92,
    51,  100, 76,  43,  81,  59,  62,  85,  33,  112,
    74,  83,  55,  50,  70,  104, 79,  65,  49,  67,
    66,  54,  118, 94,  61,  73,  95,  48,  47,  56,
    124, 106, 115, 98
};
int trit[3][3] = {
  { 1, 0, 0 },
  { 1, 0, 2 },
  { 2, 2, 1 }
};
uint16_t mem[MEMORY_SIZE];
uint16_t a = 0, c = 0, d = 0;
char* d2t(uint16_t n);
uint16_t t2d(char* str);
char* rotate(char* str);
uint16_t crz(uint16_t lhs, uint16_t rhs);
int load(FILE* f);
void exec();
int main(int argc, char** argv)
{
  FILE *f;
  if (argc != 2) {
    fprintf(stderr, "Where is the fucking file?\n");
    return 1;
  }
  if (!(f = fopen(argv[1], "r"))) {
    fprintf(stderr, "What the fuck is this file?\n");
    return 2;
  }
  load(f);
  exec();
  fclose(f);
  return 0;
}
char* d2t(uint16_t n)
{
  char *str = (char*)malloc((WIDTH + 1) * sizeof(char));
  for(int i = WIDTH - 1; i >= 0; i--) {
    str[i] = '0' + n % 3;
    n /= 3;
  }
  str[WIDTH] = '\0';
  return str;
}
uint16_t t2d(char *str)
{
  uint16_t result = 0;
  for (int i = strlen(str) - 1, j = 0; i >= 0; --i, ++j)
    result += (str[j] - '0') * pow(3, i);
  return result;
}
char* rotate(char *str)
{
  char temp = str[strlen(str) - 1];
  for (int i = strlen(str) - 1; i > 0; --i)
    str[i] = str[i - 1];
  str[0] = temp;
  return str;
}
uint16_t crz(uint16_t lhs, uint16_t rhs)
{
  char str[11] = "0000000000";
  char* lhs_s = d2t(lhs);
  char* rhs_s = d2t(rhs);
  for (int i = 0; i < WIDTH; ++i)
    str[i] = '0' + trit[lhs_s[i] - '0'][rhs_s[i] - '0'];
  return t2d(str);
}
int load(FILE* f)
{
  uint16_t i;
  char c;
  for (i = 0; i < MEMORY_SIZE;) {
    if (fread(&c, sizeof(char), 1, f) <= 0) break;
    if (isspace(c)) continue;
    switch ((c + i) % MAGIC) {
      case JMP:
      case OUT:
      case IN:
      case ROTR:
      case MOV:
      case CRZ:
      case NOP:
      case HALT:
        mem[i++] = c;
        continue;
      default:
        fprintf(stderr, "Invaild input characters\n");
        exit(1);
    }
  }
  if (i == 0) mem[0] = 1;
  if (i == 1) mem[1] = crz(0, mem[1]);
  for (i = (i < 2) ? 2 : i; i < MEMORY_SIZE; ++i)
    mem[i] = crz(mem[i - 2], mem[i - 1]);
}

void exec()
{
  int ch;
  for (;;) {
    switch ((mem[c] + c) % MAGIC) {
      case JMP:
        c = mem[d];
        break;
      case OUT:
        putc(a, stdout);
        break;
      case IN:
        ch = getc(stdin); a = (ch == EOF) ? MEMORY_SIZE - 1 : ch;
        break;
      case ROTR:
        a = mem[d] = t2d(rotate(d2t(mem[d])));
        break;
      case MOV:
        d = mem[d];
        break;
      case CRZ:
        a = mem[d] = crz(mem[d], a);
        break;
      case NOP:
        break;
      case HALT:
        return;
    }
    mem[c] = tbl[mem[c] % MAGIC];
    c = (c + 1) % MEMORY_SIZE;
    d = (d + 1) % MEMORY_SIZE;
  }
}
