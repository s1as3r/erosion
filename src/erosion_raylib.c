// clang-format off
#include "erosion_raylib.h"
#include <string.h>
// clang-format on

void erosion_init(ErosionState *state, u32 dim_x, u32 dim_y, u8 *data) {
  memset(data, 125, dim_x * dim_y * sizeof(u8));
}

void erosion_gen_data(ErosionState *state, u32 dim_x, u32 dim_y, u8 *data) {
  memset(data, 125, dim_x * dim_y * sizeof(u8));
}

bool erosion_update_state(ErosionState *state) { return false; }

void erosion_draw_ui(ErosionState *state) {}

void erosion_cleanup(ErosionState *state) {}
