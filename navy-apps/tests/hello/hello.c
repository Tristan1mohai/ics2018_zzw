#include <unistd.h>
#include <stdio.h>

int main() {
  printf("%u\n", 100);
  write(1, "Hello World!\n", 13);
  int i = 2;
  volatile int j = 0;
  while (1) {
    j ++;
    if (j == 10000) {
      printf("Hello World from Navy-apps for the %uth time!\n", i++);
      j = 0;
    }
  }
  return 0;
}
