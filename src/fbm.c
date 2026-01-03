/*
 * reference implementation: https://xbdev.net/fractals/fbm/index.php
 */
#include "fbm.h"

f32 random_f(f32 x, f32 y) {
  f32 angle = sinf(x * 12345.6789f + y * 98765.4321f + g_seed) * 43758.5453f;
  angle = angle - (f32)(i32)angle;
  angle = angle * PI * 2;
  return angle;
}

f32 dot_grid_gradient(f32 ix, f32 iy, f32 x, f32 y) {
  const f32 angle = random_f(ix, iy);
  const f32 dx = x - ix;
  const f32 dy = y - iy;
  return dx * cosf(angle) + dy * sinf(angle);
}

f32 perlin(f32 x, f32 y) {
  const f32 x0 = (f32)(i32)x;
  const f32 x1 = x0 + 1;
  const f32 y0 = (f32)(i32)y;
  const f32 y1 = y0 + 1;

  const f32 sx = smoothstep(x - x0);
  const f32 sy = smoothstep(y - y0);

  const f32 n0 = dot_grid_gradient(x0, y0, x, y);
  const f32 n1 = dot_grid_gradient(x1, y0, x, y);
  const f32 ix0 = lerp(n0, n1, sx);

  const f32 n2 = dot_grid_gradient(x0, y1, x, y);
  const f32 n3 = dot_grid_gradient(x1, y1, x, y);
  const f32 ix1 = lerp(n2, n3, sx);

  return lerp(ix0, ix1, sy);
}

f32 fbm(f32 x, f32 y, FBMParams params) {
  f32 total = 0.0f;
  f32 freq = 1.0f;
  f32 amplitude = 1.0f;
  f32 max_val = 0.0f;

  for (u32 i = 0; i < params.octaves; i++) {
    total += perlin(x * freq, y * freq) * amplitude;
    max_val += amplitude;
    freq *= params.lacunarity;
    amplitude *= params.gain;
  }
  return total / max_val;
}

void gen_fbm(u8 *heightmap_data, u32 dim_x, u32 dim_y, FBMParams params) {
  const f32 scale = 0.02f;
  for (u32 y = 0; y < dim_y; y++) {
    for (u32 x = 0; x < dim_x; x++) {
      const f32 nx = (f32)x * scale;
      const f32 ny = (f32)y * scale;

      const f32 noise = (fbm(nx, ny, params) + 1.0f) * 0.5f;
      *(heightmap_data + (y * dim_x) + x) = (u8)(noise * (f32)UINT8_MAX);
    }
  }
}
