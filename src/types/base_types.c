#include "types/base_types.h"
#include <stdlib.h>

inline f32 random_f32() {
  return ((f32) rand() / RAND_MAX);
}

inline f32 random_f32_range(f32 min, f32 max) {
  return min + ((f32) rand() / RAND_MAX) * (max - min);
}
