// clang-format off
#include "erosion_raylib.h"
#include "stdlib.h"
#include "erosion.h"
#include "fbm_raylib.h"
#include <raylib.h>
#include <stdint.h>
// clang-format on

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
  state->iterations_per_frame = DEFAULT_ITER_PER_FRAME;
  state->current_iteration = 0;
  state->is_generating = false;
  state->show_fbm_params = false;
  state->fbm_updated = false;
  state->generate_btn_clicked = false;

  state->hmap = malloc(sizeof(f32) * dim_x * dim_y);

  fbm_init(&state->fbm_state, dim_x, dim_y, data);
  inv_set_data_using_hmap(state->hmap, data, dim_x, dim_y);
  erosion_gen_data(state, dim_x, dim_y, data);

  i32 fonst_size = GuiGetStyle(DEFAULT, TEXT_SIZE);
  g_erosion_slider_x_offset =
      10.0f + (f32)MeasureText("drop lifetime: ", fonst_size);
  g_erosion_slider_x_offset = g_erosion_slider_x_offset > g_fbm_slider_x_offset
                                  ? g_erosion_slider_x_offset
                                  : g_fbm_slider_x_offset;

  g_fbm_slider_x_offset = g_erosion_slider_x_offset;
}

void erosion_gen_data(ErosionState *state, u32 dim_x, u32 dim_y, u8 *data) {
  if (state->fbm_updated) {
    fbm_gen_data(&state->fbm_state, dim_x, dim_y, data);
    inv_set_data_using_hmap(state->hmap, data, dim_x, dim_y);
  }
  if (state->is_generating) {
    for (u32 i = 0; i < state->iterations_per_frame; i++) {
      hydraulic_erosion(state->hmap, &state->params, dim_x, dim_y);
    }
    state->current_iteration += (u32)state->iterations_per_frame;
    if (state->current_iteration >= (u32)state->iterations) {
      state->is_generating = false;
    }
  }

  set_data_using_hmap(state->hmap, data, dim_x, dim_y);
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

  if (params_changed) {
    state->prev_params = state->params;
  }
  state->fbm_updated = fbm_update_state(&state->fbm_state);
  if (state->generate_btn_clicked) {
    state->is_generating = !state->is_generating;
  }

  return params_changed || state->is_generating || state->fbm_updated;
}

void _draw_erosion_only_ui(ErosionState *state) {
  GuiSlider(
      (Rectangle){
          .x = g_erosion_slider_x_offset, .y = 10, .height = 30, .width = 200},
      "iterations: ", TextFormat("%lu", (u32)state->iterations),
      &state->iterations, 1, 1000000);

  state->generate_btn_clicked =
      GuiButton((Rectangle){.x = g_erosion_slider_x_offset + 250,
                            .y = 10,
                            .height = 30,
                            .width = 70},
                state->is_generating ? "stop" : "erode");

  GuiSlider((Rectangle){.x = g_erosion_slider_x_offset,
                        .y = 40 + 1,
                        .height = 30,
                        .width = 200},
            "iter/frame: ", TextFormat("%lu", (u32)state->iterations_per_frame),
            &state->iterations_per_frame, 1, 1000);
}

void erosion_draw_ui(ErosionState *state) {
  f32 y_change_ui_button = 0;
  if (state->show_fbm_params) {
    fbm_draw_ui(&state->fbm_state);
    y_change_ui_button = 130 + 4;
  } else {
    _draw_erosion_only_ui(state);
    y_change_ui_button = 70 + 2;
  }
  bool change_ui_button_clicked =
      GuiButton((Rectangle){.x = g_erosion_slider_x_offset,
                            .y = y_change_ui_button,
                            .height = 30,
                            .width = 200},
                state->show_fbm_params ? "Erosion Params" : "FBM Params");
  if (change_ui_button_clicked) {
    state->show_fbm_params = !state->show_fbm_params;
  }
}

void erosion_cleanup(ErosionState *state) {
  fbm_cleanup(&state->fbm_state);
  free(state->hmap);
}
