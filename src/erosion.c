/*
 * reference implementation:
 * https://github.com/jovialen/hydraulic_erosion/
 */
// clang-format off
#include "erosion.h"
#include "rand.h"
#include "stdlib.h"
#include <math.h>
#include "float.h"
// clang-format on

typedef struct {
  f32 x, y;
  f32 dir_x, dir_y;
  f32 velocity;
  f32 water;
  f32 sediment;
} Drop;

f32 rand_x_position_on_hmap(u32 dim_x) {
  return pcg32_randomf() * ((f32)dim_x - 1.1f);
}

f32 rand_y_position_on_hmap(u32 dim_y) {
  return pcg32_randomf() * ((f32)dim_y - 1.1f);
}

f32 height_at(f32 *hmap, u32 x, u32 y, u32 dim_x, u32 dim_y) {
  return *(hmap + (dim_x * y) + x);
}

void set_height_at(f32 *hmap, u32 x, u32 y, u32 dim_x, f32 v) {
  *(hmap + (dim_x * y) + x) = v;
}

f32 get_drop_height(f32 *hmap, f32 x, f32 y, u32 dim_x, u32 dim_y) {
  u32 ix = (u32)x;
  u32 iz = (u32)y;
  f32 u = x - (f32)ix;
  f32 v = y - (f32)iz;

  if (ix >= dim_x - 1 || iz >= dim_y - 1) {
    return height_at(hmap, ix < dim_x ? ix : dim_x - 1,
                     iz < dim_y ? iz : dim_y - 1, dim_x, dim_y);
  }

  return height_at(hmap, ix, iz, dim_x, dim_y) * (1 - u) * (1 - v) +
         height_at(hmap, ix + 1, iz, dim_x, dim_y) * u * (1 - v) +
         height_at(hmap, ix, iz + 1, dim_x, dim_y) * (1 - u) * v +
         height_at(hmap, ix + 1, iz + 1, dim_x, dim_y) * u * v;
}

void deposit_terrain(f32 *hmap, f32 x, f32 y, f32 amount, u32 dim_x,
                     u32 dim_y) {
  u32 ix = (u32)x;
  u32 iz = (u32)y;
  f32 u = x - (f32)ix;
  f32 v = y - (f32)iz;

  if (ix >= dim_x - 1 || iz >= dim_y - 1) {
    u32 safe_x = ix < dim_x ? ix : dim_x - 1;
    u32 safe_z = iz < dim_y ? iz : dim_y - 1;
    set_height_at(hmap, safe_x, safe_z, dim_x,
                  height_at(hmap, safe_x, safe_z, dim_x, dim_y) + amount);
    return;
  }

  f32 cells[2][2] = {
      {height_at(hmap, ix, iz, dim_x, dim_y),
       height_at(hmap, ix + 1, iz, dim_x, dim_y)},
      {height_at(hmap, ix, iz + 1, dim_x, dim_y),
       height_at(hmap, ix + 1, iz + 1, dim_x, dim_y)},
  };

  set_height_at(hmap, ix, iz, dim_x,
                cells[0][0] + (amount * (1 - u) * (1 - v)));
  set_height_at(hmap, ix + 1, iz, dim_x, cells[0][1] + (amount * u * (1 - v)));
  set_height_at(hmap, ix, iz + 1, dim_x, cells[1][0] + (amount * (1 - u) * v));
  set_height_at(hmap, ix + 1, iz + 1, dim_x, cells[1][1] + (amount * u * v));
}

f32 _distance(f32 x1, f32 y1, f32 x2, f32 y2) {
  f32 dx = (x1 - x2);
  f32 dy = (y1 - y2);
  return sqrtf(dx * dx + dy * dy);
}

float erode_terrain(f32 *hmap, f32 x, f32 y, i32 radius, f32 amount, u32 dim_x,
                    u32 dim_y) {
  u32 ix = (u32)x;
  u32 iz = (u32)y;

  f32 weight_sum = 0;
  f32 *weights =
      malloc((((u32)radius + 1) * 2) * (((u32)radius + 1) * 2) * sizeof(float));

  // calculate relevant weights for the erosion
  i32 i = 0;
  for (i32 x = -radius; x <= radius; x++) {
    i32 coord_x = (i32)ix + x;
    if (coord_x < 0 || coord_x >= (i32)dim_x) {
      continue;
    }

    for (int z = -radius; z <= radius; z++) {
      int coord_z = (i32)iz + z;
      if (coord_z < 0 || coord_z >= (i32)dim_y) {
        continue;
      }

      // calculate the weight based on the distance from the erosion center
      f32 distance =
          _distance((f32)ix, (f32)iz, (f32)coord_x + 0.5f, (f32)coord_z + 0.5f);
      f32 weight = fmaxf((f32)radius - distance, 0.0f);
      weights[i] = weight;
      weight_sum += weight;
      i++;
    }
  }

  f32 eroded = 0;

  // erode the requested amount based on the calculated weights
  i = 0;
  for (i32 x = -radius; x <= radius; x++) {
    i32 coord_x = (i32)ix + x;
    if (coord_x < 0 || coord_x >= (i32)dim_x) {
      continue;
    }

    for (int z = -radius; z <= radius; z++) {
      int coord_z = (i32)iz + z;
      if (coord_z < 0 || coord_z >= (i32)dim_y) {
        continue;
      }

      // calculate the exact value to erode the current point
      float h = height_at(hmap, (u32)coord_x, (u32)coord_z, dim_x, dim_y);
      float we = amount * (weights[i] / weight_sum);
      float erode = h > we ? we : h;

      // erode
      set_height_at(hmap, (u32)coord_x, (u32)coord_z, dim_x, h - erode);
      eroded += erode;
      i++;
    }
  }

  free(weights);

  return eroded;
}

void hydraulic_erosion(f32 *hmap, const ErosionParams *params, u32 dim_x,
                       u32 dim_y) {
  // create a drop on a random position on the terrain
  Drop drop = (Drop){
      .x = rand_x_position_on_hmap(dim_x),
      .y = rand_y_position_on_hmap(dim_y),
      .water = 1.0f,
      .velocity = 1.0f,
  };

  for (int iteration = 0; iteration < params->drop_lifetime; iteration++) {
    u32 ix = (u32)drop.x;
    u32 iz = (u32)drop.y;
    f32 u = drop.x - (f32)ix;
    f32 v = drop.y - (f32)iz;

    if (ix >= dim_x - 1 || iz >= dim_y - 1) {
      break;
    }

    float neighbors[2][2] = {
        {height_at(hmap, ix, iz, dim_x, dim_y),
         height_at(hmap, ix + 1, iz, dim_x, dim_y)},
        {height_at(hmap, ix, iz + 1, dim_x, dim_y),
         height_at(hmap, ix + 1, iz + 1, dim_x, dim_y)},
    };

    // calculate the gradient of the slope the drop is currently on
    f32 grad_x = 0.0f, grad_y = 0.0f;
    grad_x = ((neighbors[0][1] - neighbors[0][0]) * (1 - v)) +
             ((neighbors[1][1] - neighbors[1][0]) * v);
    grad_y = ((neighbors[1][0] - neighbors[0][0]) * (1 - u)) +
             ((neighbors[1][1] - neighbors[0][1]) * u);

    // calculate the direction on the drop based on its current direction as
    // well as the slope
    drop.dir_x =
        (drop.dir_x * params->inertia) - (grad_x * (1 - params->inertia));
    drop.dir_y =
        (drop.dir_y * params->inertia) - (grad_y * (1 - params->inertia));

    f32 dir_len = sqrtf(drop.dir_x * drop.dir_x + drop.dir_y * drop.dir_y);
    if (dir_len > 0) {
      drop.dir_x /= dir_len;
      drop.dir_y /= dir_len;
    }

    // move the drop
    f32 old_x = drop.x, old_y = drop.y;
    drop.x += drop.dir_x;
    drop.y += drop.dir_y;

    // kill the drop if it has left the map bounds or stopped
    if (drop.x < 0.0f || drop.x >= (f32)dim_x - 1.0f || drop.y < 0.0f ||
        drop.y >= (f32)dim_y - 1.0f ||
        (drop.dir_x == 0.0f && drop.dir_y == 0.0f)) {
      break;
    }

    // find the height difference between the last and current position
    f32 height_dif = get_drop_height(hmap, drop.x, drop.y, dim_x, dim_y) -
                     get_drop_height(hmap, old_x, old_y, dim_x, dim_y);

    // calculate the capacity of the droplet based on its speed, water content
    // and the capacity modifier
    f32 capacity =
        fmaxf((-height_dif) * drop.velocity * drop.water * params->capacity,
              params->min_capacity);

    if (height_dif > 0) {
      // try to equal height
      float deposit = fminf(drop.sediment, height_dif);
      drop.sediment -= deposit;
      deposit_terrain(hmap, old_x, old_y, deposit, dim_x, dim_y);
    } else if (drop.sediment > capacity) {
      // deposit sediment
      float deposit = (drop.sediment - capacity) * params->deposition;
      drop.sediment -= deposit;
      deposit_terrain(hmap, old_x, old_y, deposit, dim_x, dim_y);
    } else {
      // erode terrain
      float erode =
          fminf((capacity - drop.sediment) * params->erosion, -height_dif);
      drop.sediment += erode_terrain(hmap, old_x, old_y, params->radius, erode,
                                     dim_x, dim_y);
    }

    // update drop velocity and water content
    drop.velocity =
        sqrtf(drop.velocity * drop.velocity + height_dif * params->gravity);
    drop.water *= (1 - params->evaporation);
  }
}
