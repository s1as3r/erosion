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
}

void erosion_gen_data(ErosionState *state, u32 dim_x, u32 dim_y, u8 *data) {
  if (state->fbm_updated) {
    fbm_gen_data(&state->fbm_state, dim_x, dim_y, data);
    inv_set_data_using_hmap(state->hmap, data, dim_x, dim_y);
  }
  if (state->is_generating) {
    for (u32 i = 0; i < (u32)state->iterations_per_frame; i++) {
      hydraulic_erosion(state->hmap, &state->params, dim_x, dim_y);
    }
    state->current_iteration += (u32)state->iterations_per_frame;
    if (state->current_iteration >= (u32)state->iterations) {
      state->is_generating = false;
      state->current_iteration = 0;
    }
  }

  set_data_using_hmap(state->hmap, data, dim_x, dim_y);
}

bool erosion_update_state(ErosionState *state) {
  state->fbm_updated = fbm_update_state(&state->fbm_state);
  if (state->generate_btn_clicked) {
    state->is_generating = !state->is_generating;
    state->current_iteration = 0;
  }

  return state->is_generating || state->fbm_updated;
}

f32 _draw_erosion_only_ui(ErosionState *state, f32 slider_x_offset) {
  f32 y_offset = 10;
  GuiSlider(
      (Rectangle){
          .x = slider_x_offset, .y = y_offset, .height = 30, .width = 200},
      "drop_lifetime: ", TextFormat("%d", (i32)state->params.drop_lifetime),
      &state->params.drop_lifetime, 1, 500);
  y_offset += 31;

  GuiSlider(
      (Rectangle){
          .x = slider_x_offset, .y = y_offset, .height = 30, .width = 200},
      "inertia: ", TextFormat("%.2f", state->params.inertia),
      &state->params.inertia, 0, 1.0f);
  y_offset += 31;

  GuiSlider(
      (Rectangle){
          .x = slider_x_offset, .y = y_offset, .height = 30, .width = 200},
      "capacity: ", TextFormat("%.2f", state->params.capacity),
      &state->params.capacity, 1, 10);
  y_offset += 31;

  GuiSlider(
      (Rectangle){
          .x = slider_x_offset, .y = y_offset, .height = 30, .width = 200},
      "min_capacity: ", TextFormat("%u", state->params.min_capacity),
      &state->params.min_capacity, 0, 1.0f);
  y_offset += 31;

  GuiSlider(
      (Rectangle){
          .x = slider_x_offset, .y = y_offset, .height = 30, .width = 200},
      "deposition: ", TextFormat("%.2f", state->params.deposition),
      &state->params.deposition, 0, 1);
  y_offset += 31;

  GuiSlider(
      (Rectangle){
          .x = slider_x_offset, .y = y_offset, .height = 30, .width = 200},
      "erosion: ", TextFormat("%.2f", state->params.erosion),
      &state->params.erosion, 0, 1);
  y_offset += 31;

  GuiSlider(
      (Rectangle){
          .x = slider_x_offset, .y = y_offset, .height = 30, .width = 200},
      "gravity: ", TextFormat("%.2f", state->params.gravity),
      &state->params.gravity, 0, 50);
  y_offset += 31;

  GuiSlider(
      (Rectangle){
          .x = slider_x_offset, .y = y_offset, .height = 30, .width = 200},
      "evaporation: ", TextFormat("%.2f", state->params.evaporation),
      &state->params.evaporation, 0, 1);
  y_offset += 31;

  GuiSlider(
      (Rectangle){
          .x = slider_x_offset, .y = y_offset, .height = 30, .width = 200},
      "radius: ", TextFormat("%d", (i32)state->params.radius),
      &state->params.radius, 0, 20);
  y_offset += 31;

  y_offset += 10;
  GuiSlider(
      (Rectangle){
          .x = slider_x_offset, .y = y_offset, .height = 30, .width = 200},
      "iterations: ", TextFormat("%u", (u32)state->iterations),
      &state->iterations, 1, 1000000);
  y_offset += 31;

  GuiSlider(
      (Rectangle){
          .x = slider_x_offset, .y = y_offset, .height = 30, .width = 200},
      "iter/frame: ", TextFormat("%u", (u32)state->iterations_per_frame),
      &state->iterations_per_frame, 1, 1000);
  y_offset += 31;

  state->generate_btn_clicked = GuiButton(
      (Rectangle){
          .x = slider_x_offset, .y = y_offset, .height = 30, .width = 200},
      state->is_generating ? "stop erosion" : "start erosion");

  y_offset += 31;
  return y_offset;
}

void erosion_draw_ui(ErosionState *state, f32 slider_x_offset) {
  f32 y_change_ui_button = 0;
  if (state->show_fbm_params) {
    fbm_draw_ui(&state->fbm_state, slider_x_offset);
    y_change_ui_button = 130 + 4;
  } else {
    y_change_ui_button = _draw_erosion_only_ui(state, slider_x_offset);
  }
  bool change_ui_button_clicked =
      GuiButton((Rectangle){.x = slider_x_offset,
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
