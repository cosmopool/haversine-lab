#include "core.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

void printUsageAndExit() {
  printf("usage: ./cli [sample_count]\n");
  exit(1);
}

i32 main(int argc, char *argv[]) {
  u64 sample_count = 0;

  // argument parsing
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
      ASSERT(num <= 200000000, "too many samples");
      sample_count = (u64)num;

      continue;
    }

    printUsageAndExit();
  }

  u64 pair_count = sample_count * 2;
  f64 *pairs = (f64 *)malloc(pair_count * sizeof(f64));
  bzero(pairs, pair_count);

  free(pairs);
  return R_SUCCESS;
}
