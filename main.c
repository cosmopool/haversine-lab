#include "core.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>

void printUsageAndExit() {
  printf("usage: ./cli [sample_count]\n");
  exit(1);
}

i32 main(int argc, char *argv[]) {
  u64 sample_count = 0;

  for (i32 i = 1; i < argc; i++) {
    if (i == 1) {
      errno = 0;
      char *end;
      const i64 num = strtol(argv[i], &end, 10);

      if (end == argv[i]) {
        printf("must provide sample_count\n");
        printUsageAndExit();
      }

      if (errno != 0) {
        perror("argument error: sample_count:");
        exit(1);
      }

      errno = 0;
      ASSERT(num >= 0, "sample count must be positive");
      ASSERT(num <= 9999999999, "too many samples");
      sample_count = (u64)num;

      continue;
    }

    printUsageAndExit();
  }
  (void)sample_count;

  return R_SUCCESS;
}
