// clang-format off
#include "erosion_raylib.h"
// clang-format on

void erosion_init(ErosionState *state, u32 dim_x, u32 dim_y, u8 *data);
void fbmsion_gen_data(ErosionState *state, u32 dim_x, u32 dim_y, u8 *data);
bool fbmsion_update_state(ErosionState *state);
void fbmsion_draw_ui(ErosionState *state);
void fbmsion_cleanup(ErosionState *state);
