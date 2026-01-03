#ifndef _H_EROSION_FBM
#define _H_EROSION_FBM

#include "base.h"
#include <math.h>

global f32 g_seed = 0.0f;

typedef struct {
  u32 octaves;
  f32 lacunarity;
  f32 gain;
} FBMParams;

f32 dot_grid_gradient(f32 ix, f32 iy, f32 x, f32 y);

f32 perlin(f32 x, f32 y);
f32 fbm(f32 x, f32 y, FBMParams params);
void gen_fbm(u8 *heightmap_data, u32 dim_x, u32 dim_y, FBMParams params);

#endif // _H_EROSION_FBM
