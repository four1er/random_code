#include <stdio.h>
#include <stdlib.h>

#include "coroutine.h"

int main() {
  struct schedule* s = malloc(sizeof(struct schedule));
  // print s addr
  printf("s addr: %p\n", s);
  printf("s.stack addr: %p\n", &(s->stack));
  printf("s.main addr: %p\n", &s->main);
  free(s);
  return 0;
}