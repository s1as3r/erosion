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
#define INITIAL_DIM_X 256
#define INITIAL_DIM_Y 256

typedef struct {
  f32 slider_x_offset;
} FBMGuiConstants;

typedef struct {
  u32 dim_x;
  u32 dim_y;
  f32 _octaves_f;
  FBMParams params;
  FBMParams prev_params;
  u8 *heightmap;

  // gui stuff
  bool seed_changed;

  // raylib stuff
  Image image;
  Texture texture;
  Mesh mesh;
  Model model;
} FBMState;

global FBMGuiConstants g_gui_constants = {0};

void fbm_init(FBMState *state) {
  state->dim_x = INITIAL_DIM_X;
  state->dim_y = INITIAL_DIM_Y;
  state->image = (Image){.height = INITIAL_DIM_Y,
                         .width = INITIAL_DIM_X,
                         .mipmaps = 1,
                         .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE};

  state->image.data = malloc(INITIAL_DIM_X * INITIAL_DIM_Y * sizeof(u8));

  state->_octaves_f = INITIAL_OCTAVES;
  state->params = (FBMParams){.octaves = (u32)INITIAL_OCTAVES,
                              .lacunarity = INITIAL_LACUNARITY,
                              .gain = INITIAL_GAIN};
  state->prev_params = state->params;

  gen_fbm((u8 *)state->image.data, state->dim_x, state->dim_y, state->params);

  state->texture = LoadTextureFromImage(state->image);
  state->mesh = GenMeshHeightmap(state->image, (Vector3){16, 8, 16});
  state->model = LoadModelFromMesh(state->mesh);
  state->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = state->texture;

  i32 fonst_size = GuiGetStyle(DEFAULT, TEXT_SIZE);
  g_gui_constants.slider_x_offset =
      10.0f + (f32)MeasureText("lacunarity: ", fonst_size);
}

void _reload_raylib_stuff(FBMState *state) {
  UnloadTexture(state->texture);
  UnloadModel(state->model);

  gen_fbm((u8 *)state->image.data, state->dim_x, state->dim_y, state->params);
  state->texture = LoadTextureFromImage(state->image);
  state->mesh = GenMeshHeightmap(state->image, (Vector3){16, 8, 16});
  state->model = LoadModelFromMesh(state->mesh);
  state->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = state->texture;

  state->prev_params = state->params;
}

void fbm_update_state(FBMState *state) {
  state->params.octaves = (u32)state->_octaves_f;
  bool params_changed =
      (state->prev_params.octaves != state->params.octaves) ||
      (state->prev_params.gain != state->params.gain) ||
      (state->prev_params.lacunarity != state->params.lacunarity);

  if (state->seed_changed) {
    g_seed = pcg32_randomf();
  }

  if (state->seed_changed || params_changed) {
    _reload_raylib_stuff(state);
  }
}

void fbm_draw_ui(FBMState *state) {
  f32 slider_x_offset = g_gui_constants.slider_x_offset;
  GuiSlider(
      (Rectangle){.x = slider_x_offset, .y = 10, .height = 30, .width = 200},
      "octaves: ", TextFormat("%d", (i32)state->_octaves_f), &state->_octaves_f,
      1, 10);

  GuiSlider(
      (Rectangle){.x = slider_x_offset, .y = 40 + 1, .height = 30, .width = 200},
      "lacunarity: ", TextFormat("%f", state->params.lacunarity),
      &state->params.lacunarity, 1, 5);

  GuiSlider(
      (Rectangle){.x = slider_x_offset, .y = 70 + 2, .height = 30, .width = 200},
      "gain: ", TextFormat("%f", state->params.gain), &state->params.gain, 0,
      1);

  state->seed_changed = GuiButton(
      (Rectangle){.x = slider_x_offset, .y = 100 + 3, .height = 30, .width = 200},
      "Change Seed");
}

void fbm_cleanup(FBMState *state) {
  UnloadTexture(state->texture);
  UnloadModel(state->model);
  free(state->image.data);
}
