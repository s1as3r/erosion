#ifndef _H_EROSION_FBM_RAYLIB
#define _H_EROSION_FBM_RAYLIB

// clang-format off
#include "base.h"
#include "fbm.h"

#include <raylib.h>
#include <raygui.h>
// clang-format on

global f32 g_fbm_slider_x_offset = 0;

typedef struct {
  f32 _octaves_f;
  FBMParams params;
  FBMParams prev_params;

  // gui stuff
  bool seed_changed;
} FBMState;

void fbm_init(FBMState *state, u32 dim_x, u32 dim_y, u8 *heightmap_data);
void fbm_gen_data(FBMState *state, u32 dim_x, u32 dim_y, u8 *data);
bool fbm_update_state(FBMState *state);
void fbm_draw_ui(FBMState *state);
void fbm_cleanup(FBMState *state);

#endif // _H_EROSION_FBM_RAYLIB
