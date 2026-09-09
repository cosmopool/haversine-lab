#include "core.h"
#include "region.h"
#include "sample.h"

#include <stdbool.h>
#include <stdio.h>

static u8 err = 0;

void printUsage(FILE *out) {
  if (!out) out = stdout;
  fprintf(out, "Usage: ./processor <samples.json> --bin [samples.bin] [OPTIONS]\n");
  fprintf(out, "\n");
}

void printUsageAndExit() {
  printUsage(stderr);
  exit(1);
}

f64 randomFloat(f64 max, f64 min, u32 *seed) {
  f64 res = ((max - min) * ((float)rand_r(seed) / RAND_MAX)) + min;
  return res;
}

i32 main(int argc, char *argv[]) {
  bool verbose = false;
  char *json_path = {0};
  char *bin_path = {0};

  if (argc == 1) {
    printUsageAndExit();
  }

  // argument parsing
  for (i32 i = 1; i < argc; i++) {
    if (strncmp(argv[i], "--help", 6) == 0 || strncmp(argv[i], "-h", 2) == 0) {
      printUsage(stdout);
      goto deinit;
    }

    if (i == 1) {
      json_path = argv[i];
      if (strlen(json_path) == 0) {
        fprintf(stderr, "invalid 'sample json path'\n\n");
        printUsageAndExit();
      }
      continue;
    }

    else if (strncmp(argv[i], "--verbose", 9) == 0 || strncmp(argv[i], "-v", 2) == 0) {
      verbose = true;
      continue;
    }

    else if (strncmp(argv[i], "--bin", 5) == 0) {
      i++;
      bin_path = argv[i];
      continue;
    }

    else {
      printUsageAndExit();
    }
  }

  FILE *json_file = {0};
  FILE *bin_file = {0};
  if (verbose) printf("--> opening json file\n");
  json_file = fopen(json_path, "rb");
  if (!json_file) {
    perror("fopen");
    err = 1;
    goto deinit;
  }
  if (bin_path) {
    if (verbose) printf("--> opening binary file\n");
    bin_file = fopen(bin_path, "rb");
    if (!json_file) {
      perror("fopen");
      err = 1;
      goto deinit;
    }
  }

  // json reading

deinit:
  if (json_file) {
    if (verbose) printf("closing json file\n");
    fclose(json_file);
  }
  if (bin_file) {
    if (verbose) printf("closing bin file\n");
    fclose(bin_file);
  }
  if (err) return R_FAILURE;
  return R_SUCCESS;
}
