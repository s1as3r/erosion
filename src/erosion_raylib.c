// clang-format off
#include "erosion_raylib.h"
#include "stdlib.h"
#include "erosion.h"
#include "fbm_raylib.h"
#include <stdint.h>
// clang-format on

global f32 g_erosion_slider_x_offset;

void set_data_using_hmap(f32 *hmap, u8 *data, u32 dim_x, u32 dim_y) {
  for (u32 i = 0; i < dim_x; i++) {
    for (u32 j = 0; j < dim_y; j++) {
      *(data + (dim_x * j) + i) =
          (u8)(*(hmap + (dim_x * j) + i) * (f32)UINT8_MAX);
    }
  }
}

void inv_set_data_using_hmap(f32 *hmap, u8 *data, u32 dim_x, u32 dim_y) {
  for (u32 i = 0; i < dim_x; i++) {
    for (u32 j = 0; j < dim_y; j++) {
      *(hmap + (dim_x * j) + i) =
          ((f32) * (data + (dim_x * j) + i)) / (f32)UINT8_MAX;
    }
  }
}

void erosion_init(ErosionState *state, u32 dim_x, u32 dim_y, u8 *data) {
  state->params = g_default_erosion_params;
  state->prev_params = state->params;
  state->iterations = DEFAULT_ITERATIONS;
  state->_iterations_f = (f32)state->iterations;

  state->hmap = malloc(sizeof(f32) * dim_x * dim_y);
  erosion_gen_data(state, dim_x, dim_y, data);

  i32 fonst_size = GuiGetStyle(DEFAULT, TEXT_SIZE);
  g_erosion_slider_x_offset =
      10.0f + (f32)MeasureText("drop lifetime: ", fonst_size);
}

void erosion_gen_data(ErosionState *state, u32 dim_x, u32 dim_y, u8 *data) {
  elog("[EROSION] generating data");
  FBMState fbm_state;
  fbm_init(&fbm_state, dim_x, dim_y, data);
  inv_set_data_using_hmap(state->hmap, data, dim_x, dim_y);

  for (u32 i = 0; i < state->iterations; i++) {
    hydraulic_erosion(state->hmap, &state->params, dim_x, dim_y);
  }

  set_data_using_hmap(state->hmap, data, dim_x, dim_y);
  elog("[EROSION] data generation finished");
}

bool erosion_update_state(ErosionState *state) {
  ErosionParams params = state->params;
  ErosionParams prev_params = state->prev_params;

#define NOT_EQUAL(param) (params.param != prev_params.param)
  bool params_changed =
      NOT_EQUAL(drop_lifetime) || NOT_EQUAL(inertia) || NOT_EQUAL(capacity) ||
      NOT_EQUAL(min_capacity) || NOT_EQUAL(deposition) || NOT_EQUAL(erosion) ||
      NOT_EQUAL(gravity) || NOT_EQUAL(evaporation) || NOT_EQUAL(radius);
#undef NOT_EQUAL

  state->iterations = (u32)state->_iterations_f;

  if (params_changed) {
    state->prev_params = state->params;
  }

  return params_changed || state->generate;
}

void erosion_draw_ui(ErosionState *state) {
  GuiSlider(
      (Rectangle){
          .x = g_erosion_slider_x_offset, .y = 10, .height = 30, .width = 200},
      "iterations: ", TextFormat("%lu", (u32)state->_iterations_f),
      &state->_iterations_f, 1, 1000000);
  state->generate = GuiButton((Rectangle){.x = g_erosion_slider_x_offset + 225,
                                          .y = 10,
                                          .height = 30,
                                          .width = 50},
                              "erode");
}

void erosion_cleanup(ErosionState *state) { free(state->hmap); }
