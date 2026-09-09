#include "core.h"
#include "haversine.c"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#define EARTH_RADIUS 6372.8
#define REGIONS_COUNT 32

typedef struct {
  f64 x0;
  f64 y0;
  f64 x1;
  f64 y1;
  f64 hs;
} Sample;

typedef struct {
  i8 x_min;
  i8 x_max;
  i8 y_min;
  i8 y_max;
} Region;

static const Region regions[REGIONS_COUNT] = {
    {.x_min = -39, .x_max = -113, .y_min = -7, .y_max = 17},
    {.x_min = 126, .x_max = 99, .y_min = -47, .y_max = 56},
    {.x_min = -8, .x_max = -92, .y_min = -32, .y_max = 64},
    {.x_min = 79, .x_max = 43, .y_min = -77, .y_max = 72},
    {.x_min = 100, .x_max = 19, .y_min = 0, .y_max = 19},
    {.x_min = -69, .x_max = 53, .y_min = -32, .y_max = 47},
    {.x_min = -127, .x_max = -118, .y_min = -42, .y_max = 69},
    {.x_min = 127, .x_max = 63, .y_min = -17, .y_max = 82},
    {.x_min = -85, .x_max = 15, .y_min = -72, .y_max = 59},
    {.x_min = 88, .x_max = 3, .y_min = -48, .y_max = 5},
    {.x_min = -17, .x_max = -103, .y_min = -66, .y_max = 48},
    {.x_min = -87, .x_max = 120, .y_min = -42, .y_max = 3},
    {.x_min = -12, .x_max = -127, .y_min = -64, .y_max = 66},
    {.x_min = -56, .x_max = 29, .y_min = -50, .y_max = 79},
    {.x_min = 118, .x_max = -96, .y_min = -58, .y_max = 61},
    {.x_min = -61, .x_max = -102, .y_min = -50, .y_max = 83},
    {.x_min = -56, .x_max = -93, .y_min = -46, .y_max = 19},
    {.x_min = 103, .x_max = -98, .y_min = -32, .y_max = 38},
    {.x_min = -38, .x_max = 55, .y_min = -49, .y_max = 20},
    {.x_min = -79, .x_max = 74, .y_min = -74, .y_max = 81},
    {.x_min = -90, .x_max = -120, .y_min = -1, .y_max = 84},
    {.x_min = -45, .x_max = 66, .y_min = -63, .y_max = 20},
    {.x_min = 104, .x_max = -125, .y_min = -78, .y_max = 71},
    {.x_min = 90, .x_max = -85, .y_min = -85, .y_max = 46},
    {.x_min = -36, .x_max = -125, .y_min = -30, .y_max = 87},
    {.x_min = 93, .x_max = 24, .y_min = -43, .y_max = 7},
    {.x_min = -96, .x_max = -109, .y_min = -38, .y_max = 68},
    {.x_min = 0, .x_max = 36, .y_min = -9, .y_max = 11},
    {.x_min = -23, .x_max = 13, .y_min = -89, .y_max = 83},
    {.x_min = 106, .x_max = 70, .y_min = -7, .y_max = 73},
    {.x_min = -75, .x_max = 81, .y_min = -28, .y_max = 8},
    {.x_min = -125, .x_max = -78, .y_min = -38, .y_max = 78},
};

void printUsageAndExit() {
  printf("usage: ./cli [sample_count]\n");
  exit(1);
}

f64 randomFloat(f64 max, f64 min, u32 *seed) {
  f64 res = ((max - min) * ((float)rand_r(seed) / RAND_MAX)) + min;
  return res;
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

  Sample *samples = (Sample *)malloc(sample_count * sizeof(Sample));
  bzero(samples, sample_count);

  // pair generation
  u32 r_idx = 0;
  u32 pair_per_region = (u32)(sample_count / REGIONS_COUNT);
  printf("pairs per region: %d\n", pair_per_region);
  printf("{\"pairs\": [\n");
  for (u32 i = 0; i < sample_count; i++) {
    // select region
    if (i % pair_per_region == 0) {
      r_idx++;
      r_idx %= REGIONS_COUNT;
    }
    Region region = regions[r_idx];

    // coordinate pair indexes
    Sample s = {0};

    // pair 1
    u32 x0_seed = region.x_max + region.x_min + (u32)sample_count + i;
    u32 y0_seed = region.y_max + region.y_min + (u32)sample_count + i;
    s.x0 = randomFloat(region.x_max, region.x_min, &x0_seed);
    s.y0 = randomFloat(region.y_max, region.y_min, &y0_seed);
    ASSERT(s.x0 >= -180 && s.x0 <= 180, "between x range");
    ASSERT(s.y0 >= -90 && s.y0 <= 90, "between y range");

    // pair 2
    u32 x1_seed = region.x_max + region.x_min + (u32)sample_count + i;
    u32 y1_seed = region.y_max + region.y_min + (u32)sample_count + i;
    s.x1 = randomFloat(region.x_max, region.x_min, &x1_seed);
    s.y1 = randomFloat(region.y_max, region.y_min, &y1_seed);
    ASSERT(s.x1 >= -180 && s.x1 <= 180, "between x range");
    ASSERT(s.y1 >= -90 && s.y1 <= 90, "between y range");

    // haversine
    s.hs = hsReferenceHaversine(s.x0, s.y0, s.x1, s.y1, EARTH_RADIUS);
    printf("{x0: %f, y0: %f, x1: %f, y1: %f, hs: %f}", s.x0, s.y0, s.x1, s.y1, s.hs);
    if (i != sample_count - 1) printf(",");
    printf("\n");

    samples[i] = s;
  }
  printf("]}");

  free(samples);
  return R_SUCCESS;
}
