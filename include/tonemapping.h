#pragma once

#include "types/color.h"

typedef enum ToneMappingOperatorType {
  CLAMP,
  REINHARD
} ToneMappingOperatorType;

typedef struct ToneMappingOperator {
  ToneMappingOperatorType type;

  f32 max_white;
} ToneMappingOperator;

ColorRGB tonemapping(ToneMappingOperator tonemapping_operator, Color color);

ColorRGB tonemapping_clamp(Color color);
ColorRGB tonemapping_reinhard(Color color, float max_white);
