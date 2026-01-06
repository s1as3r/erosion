/*
 * reference implementation: https://xbdev.net/fractals/fbm/index.php
 */
#ifndef _H_EROSION_FBM
#define _H_EROSION_FBM

#include "base.h"

global f32 g_seed = 0.0f;

typedef struct {
  f32 octaves;
  f32 lacunarity;
  f32 gain;
} FBMParams;

global FBMParams g_default_fbm_params = {
    .octaves = 5.0f, .lacunarity = 1.5f, .gain = 0.5};

f32 dot_grid_gradient(f32 ix, f32 iy, f32 x, f32 y);

f32 perlin(f32 x, f32 y);
f32 fbm(f32 x, f32 y, FBMParams params);
void gen_fbm(u8 *heightmap_data, u32 dim_x, u32 dim_y, FBMParams params);

#endif // _H_EROSION_FBM
