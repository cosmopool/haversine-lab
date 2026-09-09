#include "core.h"
#include <math.h>

static f64 hsSquare(f64 n) {
  f64 res = n * n;
  return res;
}

static f64 hsRadiansFromDegrees(f64 degrees) {
  f64 res = 0.01745329251994329577 * degrees;
  return res;
}

static f64 hsReferenceHaversine(f64 x0, f64 y0, f64 x1, f64 y1, f64 earth_radius) {
  f64 lat1 = y0;
  f64 lat2 = y1;
  f64 lon1 = x0;
  f64 lon2 = x1;

  f64 dlat = hsRadiansFromDegrees(lat2 - lat1);
  f64 dlon = hsRadiansFromDegrees(lon2 - lon1);
  lat1 = hsRadiansFromDegrees(lat1);
  lat2 = hsRadiansFromDegrees(lat2);

  f64 a = hsSquare(sin(dlat/2)) + cos(lat1)*cos(lat2)*hsSquare(sin(dlon/2));
  f64 c = 2 * asin(sqrt(a));

  f64 res = earth_radius * c;
  return res;
}
