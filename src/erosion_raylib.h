#ifndef _H_EROSION_RAYLIB
#define _H_EROSION_RAYLIB

// clang-format off
#include "base.h"
#include "erosion.h"
#include "rand.h"

#include <raylib.h>
#include <raygui.h>
// clang-format on

typedef struct {
  ErosionParams params;
  ErosionParams prev_params;
} ErosionState;

void erosion_init(ErosionState *state, u32 dim_x, u32 dim_y, u8 *data);
void fbmsion_gen_data(ErosionState *state, u32 dim_x, u32 dim_y, u8 *data);
bool fbmsion_update_state(ErosionState *state);
void fbmsion_draw_ui(ErosionState *state);
void fbmsion_cleanup(ErosionState *state);

#endif // _H_EROSION_RAYLIB
