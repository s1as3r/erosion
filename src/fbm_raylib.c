// clang-format off
#include "fbm.h"
#include "fbm_raylib.h"
#include "rand.h"
// clang-format on

global f32 g_slider_x_offset = 0;

void fbm_init(FBMState *state, u32 dim_x, u32 dim_y, u8 *heightmap_data) {
  state->params = g_default_fbm_params;
  state->_octaves_f = (f32)state->params.octaves;
  state->prev_params = state->params;

  gen_fbm(heightmap_data, dim_x, dim_y, state->params);

  i32 fonst_size = GuiGetStyle(DEFAULT, TEXT_SIZE);
  g_slider_x_offset = 10.0f + (f32)MeasureText("lacunarity: ", fonst_size);
}

void fbm_gen_data(FBMState *state, u32 dim_x, u32 dim_y, u8 *data) {
  gen_fbm(data, dim_x, dim_y, state->params);
}

bool fbm_update_state(FBMState *state) {
  state->params.octaves = (u32)state->_octaves_f;
  bool params_changed =
      (state->prev_params.octaves != state->params.octaves) ||
      (state->prev_params.gain != state->params.gain) ||
      (state->prev_params.lacunarity != state->params.lacunarity);

  if (state->seed_changed) {
    g_seed = pcg32_randomf();
  }

  if (state->seed_changed || params_changed) {
    state->prev_params = state->params;
    return true;
  }
  return false;
}

void fbm_draw_ui(FBMState *state) {
  GuiSlider(
      (Rectangle){.x = g_slider_x_offset, .y = 10, .height = 30, .width = 200},
      "octaves: ", TextFormat("%d", (i32)state->_octaves_f), &state->_octaves_f,
      1, 10);

  GuiSlider(
      (Rectangle){
          .x = g_slider_x_offset, .y = 40 + 1, .height = 30, .width = 200},
      "lacunarity: ", TextFormat("%.2f", state->params.lacunarity),
      &state->params.lacunarity, 1, 5);

  GuiSlider(
      (Rectangle){
          .x = g_slider_x_offset, .y = 70 + 2, .height = 30, .width = 200},
      "gain: ", TextFormat("%.2f", state->params.gain), &state->params.gain, 0,
      1);

  state->seed_changed = GuiButton(
      (Rectangle){
          .x = g_slider_x_offset, .y = 100 + 3, .height = 30, .width = 200},
      "Change Seed");
}

void fbm_cleanup(FBMState *state) {
  // no cleanup needed here
}
