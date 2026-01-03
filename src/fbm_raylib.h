#ifndef _H_EROSION_FBM_RAYLIB
#define _H_EROSION_FBM_RAYLIB

// clang-format off
#include "base.h"
#include "fbm.h"
#include "rand.h"

#include <raylib.h>
#include <raygui.h>
// clang-format on

#define INITIAL_LACUNARITY 0.2f
#define INITIAL_OCTAVES 5.0f
#define INITIAL_GAIN 0.5f

typedef struct {
  f32 _octaves_f;
  FBMParams params;
  FBMParams prev_params;
  u8 *heightmap;

  // gui stuff
  bool seed_changed;
} FBMState;

void fbm_init(FBMState *state, u32 dim_x, u32 dim_y, u8 *data);
void fbm_gen_data(FBMState *state, u32 dim_x, u32 dim_y, u8 *data);
bool fbm_update_state(FBMState *state);
void fbm_draw_ui(FBMState *state);
void fbm_cleanup(FBMState *state);

#endif // _H_EROSION_FBM_RAYLIB
