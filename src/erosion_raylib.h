#ifndef _H_EROSION_RAYLIB
#define _H_EROSION_RAYLIB

// clang-format off
#include "base.h"
#include "erosion.h"
#include "fbm_raylib.h"

#include <raylib.h>
#include <raygui.h>
// clang-format on

#define DEFAULT_ITERATIONS 10000.0f
#define DEFAULT_ITER_PER_FRAME 100.0f

global f32 g_erosion_slider_x_offset = 0.0f;

typedef struct {
  ErosionParams params;
  FBMState fbm_state;
  f32 *hmap;

  f32 iterations;
  u32 current_iteration;
  f32 iterations_per_frame;
  bool generate_btn_clicked;
  bool is_generating;
  bool show_fbm_params;
  bool fbm_updated;
} ErosionState;

void erosion_init(ErosionState *state, u32 dim_x, u32 dim_y, u8 *data);
void erosion_gen_data(ErosionState *state, u32 dim_x, u32 dim_y, u8 *data);
bool erosion_update_state(ErosionState *state);
void erosion_draw_ui(ErosionState *state);
void erosion_cleanup(ErosionState *state);

#endif // _H_EROSION_RAYLIB
