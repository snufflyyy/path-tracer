#include "types/base_types.h"

inline u32 pcg32(u64* state) {
    u64 oldstate = *state;
    *state = oldstate * 6364136223846793005ULL + 1u;
    u32 xorshifted = (u32)(((oldstate >> 18u) ^ oldstate) >> 27u);
    u32 rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

inline f32 random_f32(u64* state) {
  return (pcg32(state) >> 8) * (1.0f / 16777216.0f);
}

inline f32 random_f32_range(u64* state, f32 min, f32 max) {
  return min + random_f32(state) * (max - min);
}
