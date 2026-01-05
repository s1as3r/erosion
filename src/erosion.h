/*
 * reference implementation:
 * https://github.com/jovialen/hydraulic_erosion/
 */
#ifndef _H_EROSION
#define _H_EROSION

#include "base.h"

typedef struct {
  i32 drop_lifetime;
  f32 inertia;
  f32 capacity;
  f32 min_capacity;
  f32 deposition;
  f32 erosion;
  f32 gravity;
  f32 evaporation;
  i32 radius;
} ErosionParams;

global const ErosionParams g_default_erosion_params = {.drop_lifetime = 50,
                                                       .inertia = 0.05f,
                                                       .capacity = 4.0f,
                                                       .min_capacity = 0.01f,
                                                       .deposition = 0.3f,
                                                       .erosion = 0.3f,
                                                       .gravity = 4.0f,
                                                       .evaporation = 0.05f,
                                                       .radius = 3};

float erode_terrain(f32 *hmap, f32 x, f32 y, i32 radius, f32 amount, u32 dim_x,
                    u32 dim_y);

void hydraulic_erosion(f32 *hmap, const ErosionParams *params, u32 dim_x,
                       u32 dim_y);

#endif // _H_EROSION
