#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

const unsigned char xlat[] = "5z]&gqtyfr$(we4{WP)H-Zn,[%\\3dL+Q;>U!pJS72FhOA1CB6v^=I_0/8|jsb9m<.TVac`uY*MK'X~xDl}REokN:#?G\"i@";

int trit[3][3] =
{
  { 1, 0, 0 },
  { 1, 0, 2 },
  { 2, 2, 1 }
};

uint16_t crz(uint16_t lhs, uint16_t rhs);

int main(int argc, char** argv)
{
  uint16_t *mem = (uint16_t*)malloc(sizeof(uint16_t) * MEMORY_SIZE);
  uint16_t a = 0, c = 0, d = 0;

  FILE *f;
  if (argc != 2) {
    fprintf(stderr, "usage: %s <FILE>\n", argv[0]);
    return 1;
  }

  if (!(f = fopen(argv[1], "r"))) {
    fprintf(stderr, "%s: No suck file or directory\n", argv[1]);
    return 1;
  }

  uint16_t i;
  char ch;
  for (i = 0; i < MEMORY_SIZE;) {
    if (fread(&ch, sizeof(char), 1, f) <= 0) break;
    if (ch < 33 || ch > 126) continue;
    switch ((ch + i) % MAGIC) {
      case JMP:
      case OUT:
      case IN:
      case ROTR:
      case MOV:
      case CRZ:
      case NOP:
      case HALT:
        mem[i++] = ch;
        continue;
      default:
        fprintf(stderr, "Invaild input program: %d - %d - %c\n", i, ch, ch);
        return 1;
    }
  }

  if (i == 0) mem[0] = 1;
  if (i == 1) mem[1] = crz(0, mem[1]);
  for (i = (i < 2) ? 2 : i; i < MEMORY_SIZE; ++i)
    mem[i] = crz(mem[i - 2], mem[i - 1]);

  for (;;) {
    switch ((mem[c] + c) % MAGIC) {
      case JMP:
        c = mem[d];
        break;
      case OUT:
        putc(a, stdout);
        break;
      case IN:
        a = getc(stdin);
        break;
      case ROTR:
        a = mem[d] = mem[d] / 3 + (mem[d] % 3) * (MEMORY_SIZE / 3);
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
        return 1;
    }
    mem[c] = xlat[mem[c] - 33];
    c = (c + 1) % MEMORY_SIZE;
    d = (d + 1) % MEMORY_SIZE;
  }

  fclose(f);
  free(mem);

  return 0;
}

uint16_t crz(uint16_t lhs, uint16_t rhs)
{
  uint16_t r = 0, i;

  for (i = 1; i < MEMORY_SIZE; i *= 3) {
    r += trit[lhs % 3][rhs % 3] * i;
    lhs /= 3; rhs /= 3;
  }

  return r;
}
