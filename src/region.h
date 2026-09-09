#include "core.h"

#define REGIONS_COUNT 32

typedef struct {
  i8 x_min;
  i8 x_max;
  i8 y_min;
  i8 y_max;
} Region;

static const Region regions[REGIONS_COUNT] = {
    {.x_min = -113, .x_max = -39, .y_min = -7, .y_max = 17},
    {.x_min = 99, .x_max = 126, .y_min = -47, .y_max = 56},
    {.x_min = -92, .x_max = -8, .y_min = -32, .y_max = 64},
    {.x_min = 43, .x_max = 79, .y_min = -77, .y_max = 72},
    {.x_min = 19, .x_max = 100, .y_min = 0, .y_max = 19},
    {.x_min = -69, .x_max = 53, .y_min = -32, .y_max = 47},
    {.x_min = -127, .x_max = -118, .y_min = -42, .y_max = 69},
    {.x_min = 63, .x_max = 127, .y_min = -17, .y_max = 82},
    {.x_min = -85, .x_max = 15, .y_min = -72, .y_max = 59},
    {.x_min = 3, .x_max = 88, .y_min = -48, .y_max = 5},
    {.x_min = -103, .x_max = -17, .y_min = -66, .y_max = 48},
    {.x_min = -87, .x_max = 120, .y_min = -42, .y_max = 3},
    {.x_min = -127, .x_max = -12, .y_min = -64, .y_max = 66},
    {.x_min = -56, .x_max = 29, .y_min = -50, .y_max = 79},
    {.x_min = -96, .x_max = 118, .y_min = -58, .y_max = 61},
    {.x_min = -102, .x_max = -61, .y_min = -50, .y_max = 83},
    {.x_min = -93, .x_max = -56, .y_min = -46, .y_max = 19},
    {.x_min = -98, .x_max = 103, .y_min = -32, .y_max = 38},
    {.x_min = -38, .x_max = 55, .y_min = -49, .y_max = 20},
    {.x_min = -79, .x_max = 74, .y_min = -74, .y_max = 81},
    {.x_min = -120, .x_max = -90, .y_min = -1, .y_max = 84},
    {.x_min = -45, .x_max = 66, .y_min = -63, .y_max = 20},
    {.x_min = -125, .x_max = 104, .y_min = -78, .y_max = 71},
    {.x_min = -85, .x_max = 90, .y_min = -85, .y_max = 46},
    {.x_min = -125, .x_max = -36, .y_min = -30, .y_max = 87},
    {.x_min = 24, .x_max = 93, .y_min = -43, .y_max = 7},
    {.x_min = -109, .x_max = -96, .y_min = -38, .y_max = 68},
    {.x_min = 0, .x_max = 36, .y_min = -9, .y_max = 11},
    {.x_min = -23, .x_max = 13, .y_min = -89, .y_max = 83},
    {.x_min = 70, .x_max = 106, .y_min = -7, .y_max = 73},
    {.x_min = -75, .x_max = 81, .y_min = -28, .y_max = 8},
    {.x_min = -125, .x_max = -78, .y_min = -38, .y_max = 78},
};
